module Request = Express.Request;
module Response = Express.Response;
module Middleware = Express.Middleware;

type origin =
  | Boolean(bool)
  | String(string)
  | RegExp(Js.Re.t)
  | Array(array(string))
  | ArrayR(array(Js.Re.t))
  | Function((option(string), (option(Js.Exn.t), bool) => unit) => unit);

type rawOrigin;

type corsOptions = {
  origin: rawOrigin,
  methods: array(string),
  allowedHeaders: option(array(string)),
  exposedHeaders: option(array(string)),
  credentials: bool,
  maxAge: option(int),
  preflightContinue: bool,
  optionsSuccessStatus: Response.StatusCode.t,
};

let httpRawMethod: Request.httpMethod => string =
  method =>
    switch (method) {
    | Get => "GET"
    | Post => "POST"
    | Put => "PUT"
    | Patch => "PATCH"
    | Delete => "DELETE"
    | Head => "HEAD"
    | Options => "OPTIONS"
    | Trace => "TRACE"
    | Connect => "CONNECT"
    };

[@bs.val]
external corsRawOrigin: origin => rawOrigin = "Array.prototype.shift.call";
[@bs.module] external cors_: corsOptions => Middleware.t = "cors";
[@bs.module]
external corsAsync:
  (Request.t, (option(Js.Exn.t), corsOptions) => unit) => unit =
  "cors";

let defaultOptions = {
  origin: String("*") |> corsRawOrigin,
  methods:
    Array.map(
      httpRawMethod,
      Request.([|Get, Head, Put, Patch, Post, Delete|]),
    ),
  allowedHeaders: None,
  exposedHeaders: None,
  credentials: false,
  maxAge: None,
  preflightContinue: false,
  optionsSuccessStatus: Response.StatusCode.NoContent,
};
let cors =
    (
      ~origin=String("*"),
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
      (),
    ) => {
  cors_({
    origin: corsRawOrigin(origin),
    methods: Array.map(httpRawMethod, methods),
    allowedHeaders,
    exposedHeaders,
    credentials,
    maxAge,
    preflightContinue,
    optionsSuccessStatus,
  });
};