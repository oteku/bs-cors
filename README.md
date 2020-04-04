# bs-cors

bs-cors is a Buckelscript binding to [cors](https://github.com/expressjs/cors), made by [Troy Goode](https://twitter.com/intent/user?screen_name=troygoode), an expressjs middleware that can be used to enable [Cross-origin resource sharing](https://developer.mozilla.org/fr/docs/Web/HTTP/CORS) with various options.

- [Installation](#installation)
- [Usage](#usage)
  - [Simple Usage](#simple-usage-enable-all-cors-requests)
  - [Enable CORS for a Single Route](#enable-cors-for-a-single-route)
  - [Configuring CORS](#configuring-cors)
  - [Configuring CORS w/ Dynamic Origin](#configuring-cors-w-dynamic-origin)
  - [Enabling CORS Pre-Flight](#enabling-cors-pre-flight)
  - [Configuring CORS Asynchronously](#configuring-cors-asynchronously)
- [Configuration Options](#configuration-options)
- [License](#license)
- [Author](#author)

## Installation

This is a [Node.js](https://nodejs.org/en/) module available through the
[npm registry](https://www.npmjs.com/). Installation is done using the
[`npm install` command](https://docs.npmjs.com/getting-started/installing-npm-packages-locally):

```sh
$ npm install bs-cors
```

then add it as a dependency to your **bs-config.json** :

```
...
  "bs-dependencies": ["bs-express", "bs-cors", ...],
...

```

## Usage

### Simple Usage (Enable _All_ CORS Requests)

```reasonml
open  Express;
let app = express();

App.use(app,Cors.cors());


Middleware.from((_next, _req) => {
      [("msg", Js.Json.string("This is CORS-enabled for only example.com !"))]
    |> Js.Dict.fromList
    |> Js.Json.object_
    |> Response.sendJson
})
|> App.get(app, ~path="/products/:id");

let onListen = e =>
  switch (e) {
  | exception (Js.Exn.Error(e)) =>
    (
      switch (Js.Exn.message(e)) {
      | None => "UNKNOWN ERROR"
      | Some(msg) => msg
      }
    )
    |> Js.log;
    Node.Process.exit(1);
  | _ => "CORS-enabled web server listening on port 8080" |> Js.log
  };

let server = App.listen(app, ~port=8080, ~onListen, ());
```

### Enable CORS for a Single Route

```reasonml
open Express;
let app = express();

[|
  Cors.cors(),
  Middleware.from((_next, _req) => {
    [("msg", Js.Json.string("This is CORS-enabled for only example.com !"))]
    |> Js.Dict.fromList
    |> Js.Json.object_
    |> Response.sendJson
  }),
|]
|> App.getWithMany(app, ~path="/products/:id");

let onListen = e =>
  switch (e) {
  | exception (Js.Exn.Error(e)) =>
    (
      switch (Js.Exn.message(e)) {
      | None => "UNKNOWN ERROR"
      | Some(msg) => msg
      }
    )
    |> Js.log;
    Node.Process.exit(1);
  | _ => "CORS-enabled web server listening on port 8080" |> Js.log
  };

let server = App.listen(app, ~port=8080, ~onListen, ());
```

### Configuring CORS

```reasonml
open Express;
let app = express();

[|
  Cors.cors(
    ~origin=Cors.String("example.com"),
    ~optionsSuccessStatus=Response.StatusCode.Accepted,
    (),
  ), // some legacy browsers (IE11, various SmartTVs) choke on 204
  Middleware.from((_next, _req) => {
    [("msg", Js.Json.string("This is CORS-enabled for only example.com !"))]
    |> Js.Dict.fromList
    |> Js.Json.object_
    |> Response.sendJson
  }),
|]
|> App.getWithMany(app, ~path="/products/:id");


let onListen = e =>
  switch (e) {
  | exception (Js.Exn.Error(e)) =>
    (
      switch (Js.Exn.message(e)) {
      | None => "UNKNOWN ERROR"
      | Some(msg) => msg
      }
    )
    |> Js.log;
    Node.Process.exit(1);
  | _ => "CORS-enabled web server listening on port 8080" |> Js.log
  };

let server = App.listen(app, ~port=8080, ~onListen, ());
```

### Configuring CORS w/ Dynamic Origin

```reasonML

open Express;
let app = express();

let whitelist = [|"http:\/\/example1.com", "http:\/\/example2.com"|];
let originFunction:
  (option(string), (option(Js.Exn.t), bool) => unit) => unit =
  (origin, callback) =>
    switch (origin) {
    /* If you do not want to block REST tools or server-to-server requests */
    | None => callback(None, true)
    | Some(o) =>
      if (Array.exists(item => item === o, whitelist)) {
        callback(None, true);
      } else {
        callback(Js.Exn.raiseError("Not allowed by CORS"), false);
      }
    };

[|
  Cors.cors(~origin=Function(originFunction), ()),
  Middleware.from((_next, _req) => {
    [("msg", Js.Json.string("This is CORS-enabled for only example.com !"))]
    |> Js.Dict.fromList
    |> Js.Json.object_
    |> Response.sendJson
  }),
|]
|> App.getWithMany(app, ~path="/products/:id");

let onListen = e =>
  switch (e) {
  | exception (Js.Exn.Error(e)) =>
    (
      switch (Js.Exn.message(e)) {
      | None => "UNKNOWN ERROR"
      | Some(msg) => msg
      }
    )
    |> Js.log;
    Node.Process.exit(1);
  | _ => "CORS-enabled web server listening on port 8080" |> Js.log
  };

let server = App.listen(app, ~port=8080, ~onListen, ());
```

### Enabling CORS Pre-Flight

Certain CORS requests are considered 'complex' and require an initial
`OPTIONS` request (called the "pre-flight request"). An example of a
'complex' CORS request is one that uses an HTTP verb other than
GET/HEAD/POST (such as DELETE) or that uses custom headers. To enable
pre-flighting, you must add a new OPTIONS handler for the route you want
to support:

```reasonml
open Express;
let app = express();

App.options(app, ~path="/products/:id", Cors.cors());
[|
  Cors.cors(),
  Middleware.from((_next, _req) => {
    [("msg", Js.Json.string("This is CORS-enabled for all origins!"))]
    |> Js.Dict.fromList
    |> Js.Json.object_
    |> Response.sendJson
  }),
|]
|> App.deleteWithMany(app, ~path="/products/:id");


let onListen = e =>
  switch (e) {
  | exception (Js.Exn.Error(e)) =>
    (
      switch (Js.Exn.message(e)) {
      | None => "UNKNOWN ERROR"
      | Some(msg) => msg
      }
    )
    |> Js.log;
    Node.Process.exit(1);
  | _ => "CORS-enabled web server listening on port 8080" |> Js.log
  };
```

You can also enable pre-flight across-the-board like so:

```reasonml
/* include before other routes */
App.options(app, ~path="*", Cors.cors());
```

### Configuring CORS Asynchronously

TODO

## Configuration Options

The type of the `cors` middleware function is :

```reasonml
let cors:
  (
    ~origin: origin=?,
    ~methods: array(Express.Request.httpMethod)=?,
    ~allowedHeaders: option(array(string))=?,
    ~exposedHeaders: option(array(string))=?,
    ~credentials: bool=?,
    ~maxAge: option(int)=?,
    ~preflightContinue: bool=?,
    ~optionsSuccessStatus: Express.Response.StatusCode.t=?,
    unit
  ) =>
  Express.Middleware.t;
```

- `origin`: Configures the **Access-Control-Allow-Origin** CORS header. It is a variant with thoses constructors:
  - `Boolean(bool)` - set `origin` to `Boolean(true)` to reflect the [request origin](http://tools.ietf.org/html/draft-abarth-origin-09), as defined by `req.header('Origin')`, or set it to `Boolean(false)` to disable CORS.
  - `String(string)` - set `origin` to a specific origin. For example if you set it to `String("http:\/\/example.com")` only requests from "http://example.com" will be allowed.
  - `RegExp(Js.Re.t)` - set `origin` to a regular expression pattern which will be used to test the request origin. If it's a match, the request origin will be reflected. For example the pattern `RegExp([%re "/example\.com$/"])` will reflect any request that is coming from an origin ending with "example.com".
  - `Array(array(string))` - set `origin` to an array of valid origins using strings.
  - `Array(array(Js.Re.t))` - set `origin` to an array of valid origins using RegExp.
  - `Function((option(string), (option(Js.Exn.t), bool) => unit) => unit)` - set `origin` to a function implementing some custom logic. The function takes the request origin as the first parameter and a callback
- `methods`: Configures the **Access-Control-Allow-Methods** CORS header. Expects an array (ex: `Express.Request.([|Get, Put, Post|])`).
- `allowedHeaders`: Configures the **Access-Control-Allow-Headers** CORS header. Expects an optional array (ex: `Some([|"Content-Type", "Authorization"|])`). If not specified, defaults to reflecting the headers specified in the request's **Access-Control-Request-Headers** header.
- `exposedHeaders`: Configures the **Access-Control-Expose-Headers** CORS header. Expects an optional array (ex: `Some(["Content-Range", "X-Content-Range"])`). If not specified, no custom headers are exposed.
- `credentials`: Configures the **Access-Control-Allow-Credentials** CORS header. Set to `true` to pass the header, or `false` to be omitted.
- `maxAge`: Configures the **Access-Control-Max-Age** CORS header. Set to an optional integer to pass the header, otherwise it is omitted.
- `preflightContinue`: Pass the CORS preflight response to the next handler.
- `optionsSuccessStatus`: Provides a status code to use for successful `OPTIONS` requests, since some legacy browsers (IE11, various SmartTVs) choke on `204`.

The default configuration is the equivalent of:

```reasonml
cors( ~origin=String("*"),
      ~methods=[|
                 Request.Get,
                 Request.Head,
                 Request.Put,
                 Request.Patch,
                 Request.Post,
                 Request.Delete,
               |],
      ~allowedHeaders=None,
      ~exposedHeaders=None,
      ~credentials=false,
      ~maxAge=None,
      ~preflightContinue=false,
      ~optionsSuccessStatus=Response.StatusCode.NoContent,
      ()
);
```

## License

[MPL-2.0](https://opensource.org/licenses/MPL-2.0)

## Author

[Thomas Haessle](https://github.com/oteku)

## Thanks to

[Troy Goode](https://github.com/TroyGoode) for building cors !!!
