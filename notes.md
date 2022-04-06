https://www.hbci-zka.de/spec/3_0.htm

https://www.hbci-zka.de/spec/xmlschema/ //4.0


### why is everything a pointer?
currently the AST has referencing patterns (e.g. QName). If you have a better idea how to get rid of it, please show me. The problem is definitely the reference invalidation in the copy constructors, when using reference_wrapper or anything.
