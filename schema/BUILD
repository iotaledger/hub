genrule(name="headers",
        srcs=["ddl2cpp", "schema.sql"],
        outs=["schema.h"],
        cmd="""
      $(location ddl2cpp) $(location schema.sql) schema iota::db::sql
      cp schema.h $@
        """)

cc_library(name="schema",
           hdrs=[":headers"],
           data=[":schema.sql"],
           visibility=["//visibility:public"])
