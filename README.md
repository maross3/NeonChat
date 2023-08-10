# NeonChat
C++ authoritative chat server and client dll to send data to all clients via call back. Originally intended to interop easily with C#.

For now, the project needs to be built, and the .dll placed into your unity project. Include the `UnityNeonChatClient.cs` in your project as well. The build process looks rough right now, but here's the approach:

* Decouple the build process by using docker, spinning up a container from the built image. This establishes the pipeline for our load balancing.
* Compile an updated `UnityChatClient.cs` file as a post build step. The location should be easily changable, so a user can build directly to their unity projects for development.
* Create a `/UnityNeonChat` directory, which could be remapped to the unity project as well. This will include both the `.dll` and the compiled `UnityChatClient.cs` file. Every build will update your project accordingly. This will only be on release builds. Debug builds could create a new debug directory somewhere else.

I will be putting this on my physical server soon, and opening it to the internet for live testing soon :)


![image](https://github.com/maross3/NeonChat/assets/20687907/dc328e16-e6fa-417f-ad5c-f5e268b5fc32)
