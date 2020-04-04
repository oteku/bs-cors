type origin =
  | Boolean(bool)
  | String(string)
  | RegExp(Js.Re.t)
  | Array(array(string))
  | ArrayR(array(Js.Re.t))
  | Function((option(string), (option(Js.Exn.t), bool) => unit) => unit);

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