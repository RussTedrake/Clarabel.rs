load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_import")

exports_files = [
    "clarabel.BUILD.bazel",
]

cc_binary(
    name = "example_lp",
    srcs = ["example_lp.cc"],
    deps = ["@clarabel"],
)
