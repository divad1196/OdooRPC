# C++ OdooRPC

**author**: Gallay David

### Goals

* lightweight
* minimal
* few dependencies (libcurl and std, that's all)
* but still simple



### future developments

I will look forward to add some features as:

* shortcuts fonctions (create, search, write, ...)
* Add Model class to wrap _name and ids



Support for json will probably never be added. Any third-party library can wrap this library outputs.
On the other hand, another library wrapping this one might appear.



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

   This will automatically authenticate the user and retrieve it's uid.
   The uid is related to the database, it is stored into the RPC client.
   It can be force using **forceUid**, and currently, this is the only way to use the librairy.

2. use **raw_query** method

   This will return the request body as a string.

   It could be parsed using any json library, for example [nlohmann/json](https://github.com/nlohmann/json/) which is header-only

   ```c++
   std::cout << client.raw_query(
       "res.partner",
       "search",
       {
       	"[]"
       }
   ) << std::endl;
   ```

   We can also manually  create our jsonrpc body if really needed

