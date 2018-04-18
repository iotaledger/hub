cc_library(name="commands",
           srcs=glob(["**/*.cc"],
                     exclude=["tests/**/*"]),
           hdrs=glob(
               ["**/*.h"],
               exclude=["tests/**/*"]),
           deps=["//proto:cpp", "//hub/stats:stats", "//hub/db:db"],
           visibility=["//visibility:public"])

cc_test(name="tests",
        deps=[":commands", "@com_google_googletest//:gtest"],
        srcs=glob(["tests/**/*.cc", "tests/**/*.h"]), )
