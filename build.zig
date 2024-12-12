const std = @import("std");

pub fn build(b: *std.Build) void {
    // Create a new executable target.
    const exe = b.addExecutable(.{
        .name = "server",
        .root_source_file = b.path("server.c"),
        .target = b.host,
    });

    // Specify additional libraries.
    exe.linkSystemLibrary("Ws2_32");

    // Add required definitions.
    exe.define("WIN32_LEAN_AND_MEAN", null);
    exe.define("UNICODE", null);

    // Configure the compiler for Windows
    exe.setTarget(.{ .os_tag = .windows, .arch = .x86_64 });

    // Install the executable in the output directory.
    exe.install();
    b.installArtifact(exe);

    b.default_step.dependOn(&exe.step);
}
