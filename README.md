# C++ OdooRPC

**author**: Gallay David

### Goals

* lightweight
* minimal
* few dependencies (libcurl and std, that's all)
* but still simple



### future developments

>  I will look forward to add some features as:
>
> * shortcuts fonctions (create, search, write, ...)
> * Add Model class to wrap _name and ids
>
> Support for json will probably never be added. Any third-party library can wrap this library outputs.
> On the other hand, another library wrapping this one might appear.

Taking a step back, this won't be implemented in this library.
This will stay as minimal as possible to avoid becoming opinionated.
Another library will be created based on this one for higher level use

### Use

1. Create a client

   ```c++

   const std::string URL = "my-url.com";
   const std::string DATABASE = "my-database";

   // Create Credentials
   Credentials creds("login", "password");

   OdooRPC client (
       URL,
       DATABASE,
       creds
   );
   ```

   could be written all-in-one

   ```c++
   OdooRPC client (
       "my-url.com",
       "my-database",
       {
       	"login",
       	"password"
       }
   );
   ```

   This will automatically authenticate the user and retrieve it's uid which is needed for most of the queries.
   The uid is related to the database, it is stored into the RPC client.
   
2. use **raw_query** method

   This will return the request body as a string.
   It could be parsed using any json library, for example [nlohmann/json](https://github.com/nlohmann/json/) which is header-only.



   Knowing the follwing function in python exists:

   ```python
   def search_read(self, domain=None, fields=None, offset=0, limit=None, order=None):
   	...
   ```

   We can use it so:

   ```c++
   std::cout << client.raw_query(
       "res.partner",
       "search_read",
       {
           "[]",                        // domain
           R"(["name", "user_id"])",    // fields
           0,                           // offset
           5                            // limit
       }
   ) << std::endl;
   ```

   It asks for fields "name" and "user_id" for the 5 first partners starting at offset 0 in the database with default order

   * Args are set in the same order as for the function in python.
   * Note the use of C++11 Raw-string-literals, params muse be given in json format

   We can also manually  create our jsonrpc body if really needed

