env = Environment()

env["CPPFLAGS"] = ["-std=c++0x", "-O2", "-Wall", "-pg"]
env["CPPPATH"] = ["./src/"]

env["LINKFLAGS"] = ["-pg"]

env.Program("phantom_benchmark_aggregator",
    Glob("src/*.cpp")
)
