using System;
class DockerPilot {
  static int Main(string[] args) {
    if (args.Length != 3 || args[0] != "info" || args[1] != "--format" || args[2] != "{{.ServerVersion}}") {
      Console.Error.WriteLine("Pilot shim accepts only docker info version query."); return 2;
    }
    if (Environment.GetEnvironmentVariable("MEM_TRIGGER_PILOT_DOCKER_STATE") == "ready") {
      Console.WriteLine("27.0.0"); return 0;
    }
    Console.Error.WriteLine("Controlled pilot: Docker Engine unavailable."); return 1;
  }
}
