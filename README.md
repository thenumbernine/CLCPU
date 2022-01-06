Here's my attempt at a multithreaded CPU implementation of OpenCL on the CPU.

After doing this in LuaJIT https://github.com/thenumbernine/cl-cpu-lua
I had success enough to get my hydrodynamics-in-OpenCL-in-LuaJIT project https://github.com/thenumbernine/hydro-cl-lua
working, however I did it all single-threaded (cue Lua criticism) and it was still going pretty slow, 
so I figured I'd redo it in C++ with multithreading.
