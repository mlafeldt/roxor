const std = @import("std");

const PREVIEW_LEN: usize = 50;

const Match = struct {
    offset: usize,
    key: u8,
    preview: [:0]const u8,
};

pub fn main() !void {
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();
    const allocator = arena.allocator();

    const stdout = std.io.getStdOut().writer();
    const stderr = std.io.getStdErr().writer();

    const args = try std.process.argsAlloc(allocator);
    if (args.len < 3) {
        try stderr.print("usage: {s} <file> <crib>\n", .{args[0]});
        std.os.exit(1);
    }

    const path = args[1];
    const crib = args[2];

    const file = try std.fs.cwd().openFileZ(path, .{});
    const ciphertext = try file.readToEndAlloc(allocator, std.math.maxInt(usize));

    if (ciphertext.len < crib.len) {
        try stderr.print("error: ciphertext too short\n", .{});
        std.os.exit(1);
    }

    for (try attackCipher(ciphertext, crib, allocator)) |match| {
        try stdout.print("Found text at 0x{x} (XOR key 0x{x:0>2})\n", .{ match.offset, match.key });
        try stdout.print("  preview: {s}\n", .{match.preview});
    }
}

fn attackCipher(ciphertext: []const u8, crib: []const u8, allocator: std.mem.Allocator) ![]Match {
    var matches = std.ArrayList(Match).init(allocator);

    for (ciphertext, 0..) |x, i| {
        var old_key = x ^ crib[0];
        var j: usize = 1;
        while (j < crib.len and (i + j) < ciphertext.len) {
            const key = ciphertext[i + j] ^ crib[j];
            if (key != old_key) break;
            old_key = key;
            j += 1;
            if (j == crib.len) {
                var preview = try allocator.allocSentinel(u8, @min(PREVIEW_LEN, ciphertext.len - i), 0);
                j = 0;
                while (j < PREVIEW_LEN and (i + j) < ciphertext.len) : (j += 1) {
                    const ch = ciphertext[i + j] ^ key;
                    preview[j] = if (std.ascii.isPrint(ch)) ch else '.';
                }
                try matches.append(.{ .offset = i, .key = key, .preview = preview });
            }
        }
    }

    return matches.items;
}

test attackCipher {
    const testing = std.testing;

    const tests = [_]struct {
        ciphertext: []const u8,
        crib: []const u8,
        matches: []const Match,
    }{
        .{
            .ciphertext = "",
            .crib = "",
            .matches = &.{},
        },
        .{
            .ciphertext = "haystack",
            .crib = "needle",
            .matches = &.{},
        },
        .{
            .ciphertext = "needle in haystack",
            .crib = "needle",
            .matches = &.{
                .{ .offset = 0, .key = 0, .preview = "needle in haystack" },
            },
        },
        .{
            .ciphertext = "a needle, another needle",
            .crib = "needle",
            .matches = &.{
                .{ .offset = 2, .key = 0, .preview = "needle, another needle" },
                .{ .offset = 18, .key = 0, .preview = "needle" },
            },
        },
        .{
            .ciphertext = "\x23\x62\x2c\x27\x27\x26\x2e\x27\x6e\x62\x23\x2c\x2d\x36\x2a\x27\x30\x62\x2c\x27\x27\x26\x2e\x27",
            .crib = "needle",
            .matches = &.{
                .{ .offset = 2, .key = 0x42, .preview = "needle, another needle" },
                .{ .offset = 18, .key = 0x42, .preview = "needle" },
            },
        },
    };

    for (tests) |t| {
        const matches = try attackCipher(t.ciphertext, t.crib, std.heap.page_allocator);
        try testing.expectEqual(t.matches.len, matches.len);
        for (t.matches, matches) |want, got| {
            try testing.expectEqualDeep(want, got);
        }
    }
}
