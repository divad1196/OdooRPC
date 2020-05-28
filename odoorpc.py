data = self.json(
    '/web/session/authenticate',
    {'db': db, 'login': login, 'password': password},
)

def json(url, params):
    data = self._connector.proxy_json(url, params)
    if data.get('error'):
        raise error.RPCError(
            data['error']['data']['message'], data['error']
        )
    return data


proxy_json = jsonrpclib.ProxyJSON(
    self.host,
    self.port,
    self._timeout,
    ssl=self.ssl,
    deserialize=self.deserialize,
    opener=self._opener,
)

class ProxyJSON(Proxy):
    """The :class:`ProxyJSON` class provides a dynamic access
    to all JSON methods.
    """

    def __init__(
        self, host, port, timeout=120, ssl=False, opener=None, deserialize=True
    ):
        Proxy.__init__(self, host, port, timeout, ssl, opener)
        self._deserialize = deserialize

    def __call__(self, url, params=None):
        if params is None:
            params = {}
        data = {
            "jsonrpc": "2.0",
            "method": "call",
            "params": params,
            "id": random.randint(0, 1000000000),
        }
        if url.startswith('/'):
            url = url[1:]
        full_url = self._get_full_url(url)
        log_data = get_json_log_data(data)
        logger.debug(LOG_JSON_SEND_MSG, {'url': full_url, 'data': log_data})
        data_json = json.dumps(data)
        request = Request(url=full_url, data=encode_data(data_json))
        request.add_header('Content-Type', 'application/json')
        response = self._opener.open(request, timeout=self._timeout)
        if not self._deserialize:
            return response
        result = json.load(decode_data(response))
        logger.debug(
            LOG_JSON_RECV_MSG,
            {'url': full_url, 'data': log_data, 'result': result},
        )
        return result

# Quand service == "object"
def dispatch(method, params):
    (db, uid, passwd ) = params[0], int(params[1]), params[2]

    # set uid tracker - cleaned up at the WSGI
    # dispatching phase in odoo.service.wsgi_server.application
    threading.current_thread().uid = uid

    params = params[3:]
    if method == 'obj_list':
        raise NameError("obj_list has been discontinued via RPC as of 6.0, please query ir.model directly!")
    if method not in ['execute', 'execute_kw']:
        raise NameError("Method not available %s" % method)
    security.check(db,uid,passwd)
    registry = odoo.registry(db).check_signaling()
    fn = globals()[method]
    with registry.manage_changes():
        res = fn(db, uid, *params)
    return res

    @classmethod
    def _dispatch(cls):
        # locate the controller method
        try:
            rule, arguments = cls._find_handler(return_rule=True)
            func = rule.endpoint
        except werkzeug.exceptions.NotFound as e:
            return cls._handle_exception(e)

        # check authentication level
        try:
            auth_method = cls._authenticate(func.routing["auth"]) #################################################
        except Exception as e:
            return cls._handle_exception(e)

        processing = cls._postprocess_args(arguments, rule)
        if processing:
            return processing

        # set and execute handler
        try:
            request.set_handler(func, arguments, auth_method)
            result = request.dispatch()
            if isinstance(result, Exception):
                raise result
        except Exception as e:
            return cls._handle_exception(e)

        return result
    

    @classmethod
    def _auth_method_user(cls):
        request.uid = request.session.uid
        if not request.uid:
            raise http.SessionExpiredException("Session expired")

    @classmethod
    def _authenticate(cls, auth_method='user'):
        try:
            if request.session.uid:
                try:
                    request.session.check_security()
                    # what if error in security.check()
                    #   -> res_users.check()
                    #   -> res_users._check_credentials()
                except (AccessDenied, http.SessionExpiredException):
                    # All other exceptions mean undetermined status (e.g. connection pool full),
                    # let them bubble up
                    request.session.logout(keep_db=True)
            if request.uid is None:
                getattr(cls, "_auth_method_%s" % auth_method)()
        except (AccessDenied, http.SessionExpiredException, werkzeug.exceptions.HTTPException):
            raise
        except Exception:
            _logger.info("Exception during request Authentication.", exc_info=True)
            raise AccessDenied()
        return auth_method


def dispatch(self, environ, start_response):
        """
        Performs the actual WSGI dispatching for the application.
        """
        try:
            httprequest = werkzeug.wrappers.Request(environ)
            httprequest.app = self
            httprequest.parameter_storage_class = werkzeug.datastructures.ImmutableOrderedMultiDict
            threading.current_thread().url = httprequest.url
            threading.current_thread().query_count = 0
            threading.current_thread().query_time = 0
            threading.current_thread().perf_t0 = time.time()

            explicit_session = self.setup_session(httprequest)
            self.setup_db(httprequest)
            self.setup_lang(httprequest)

            request = self.get_request(httprequest)

            def _dispatch_nodb():
                try:
                    func, arguments = self.nodb_routing_map.bind_to_environ(request.httprequest.environ).match()
                except werkzeug.exceptions.HTTPException as e:
                    return request._handle_exception(e)
                request.set_handler(func, arguments, "none")
                result = request.dispatch()
                return result

            with request:
                db = request.session.db
                if db:
                    try:
                        odoo.registry(db).check_signaling()
                        with odoo.tools.mute_logger('odoo.sql_db'):
                            ir_http = request.registry['ir.http']
                    except (AttributeError, psycopg2.OperationalError, psycopg2.ProgrammingError):
                        # psycopg2 error or attribute error while constructing
                        # the registry. That means either
                        # - the database probably does not exists anymore
                        # - the database is corrupted
                        # - the database version doesnt match the server version
                        # Log the user out and fall back to nodb
                        request.session.logout()
                        # If requesting /web this will loop
                        if request.httprequest.path == '/web':
                            result = werkzeug.utils.redirect('/web/database/selector')
                        else:
                            result = _dispatch_nodb()
                    else:
                        result = ir_http._dispatch() ###############################################################
                else:
                    result = _dispatch_nodb()

                response = self.get_response(httprequest, result, explicit_session)
            return response(environ, start_response)

        except werkzeug.exceptions.HTTPException as e:
            return e(environ, start_response)

    def get_db_router(self, db):
        if not db:
            return self.nodb_routing_map
        return request.registry['ir.http'].routing_map()

        
def execute_kw(db, uid, obj, method, args, kw=None):
    return execute(db, uid, obj, method, *args, **kw or {})

@check
def execute(db, uid, obj, method, *args, **kw):
    threading.currentThread().dbname = db
    with odoo.registry(db).cursor() as cr:
        check_method_name(method)
        res = execute_cr(cr, uid, obj, method, *args, **kw)
        if res is None:
            _logger.info('The method %s of the object %s can not return `None` !', method, obj)
        return res

def setup_session(self, httprequest):
        # recover or create session
        session_gc(self.session_store)

        sid = httprequest.args.get('session_id')
        explicit_session = True
        if not sid:
            sid =  httprequest.headers.get("X-Openerp-Session-Id")
        if not sid:
            sid = httprequest.cookies.get('session_id')
            explicit_session = False
        if sid is None:
            httprequest.session = self.session_store.new()
        else:
            httprequest.session = self.session_store.get(sid)
        return explicit_session