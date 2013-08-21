env = Environment()

env["CPPFLAGS"] = ["-std=c++0x", "-O2", "-Wall", "-g"]
env["CPPPATH"] = ["./src/", "./contrib/gmock/"]

#env["LINKFLAGS"] = ["-g"]

pba = env.Library("pba",
	["src/stats.cpp", "src/phout_reader.cpp"]
)

env.Program("phantom_benchmark_aggregator",
	["src/main.cpp"],
	LIBS=[pba]
)

env.Program("run_ut",
	Glob("ut/*.cpp") +
	Glob("contrib/gmock/*.cc"),
	LIBS=[pba, "pthread"]
)
