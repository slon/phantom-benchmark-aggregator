env = Environment()

env["CPPFLAGS"] = ["-std=c++0x", "-O2", "-Wall"]
env["CPPPATH"] = ["./src/"]

env.Program("phantom_benchmark_aggregator",
    Glob("src/*.cpp")
)
