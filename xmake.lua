set_project("muser")
set_version("1.0.0")

-- ==========================================
-- Global Settings
-- ==========================================

set_languages("c17", "cxx17")
set_warnings("all")
set_symbols("debug")

-- Output directory (equivalent to EXECUTABLE_OUTPUT_PATH)
set_targetdir("build/bin")

-- Add pthread for GNU/Clang
if is_plat("linux") then
    add_cxflags("-pthread")
    add_ldflags("-pthread")
end

-- ==========================================
-- Target
-- ==========================================

target("muser")
    set_kind("binary")

    -- Source files (recursive)
    add_files("src/**.cpp")
    add_files("src/**.cc")
    add_files("src/**.c")

    -- Include directories
    add_includedirs("src/headers")
    add_includedirs("lib/raylib/include")
    add_includedirs("lib/raygui/include")
    add_includedirs("lib/AudioFile")

    -- Link raylib static library
    add_links("raylib")
    add_linkdirs("lib/raylib")

    -- On Linux, raylib usually needs these system libs
    if is_plat("linux") then
        add_syslinks("pthread", "dl", "m", "rt", "X11")
    end

    -- ======================================
    -- Copy resources after build
    -- ======================================
    after_build(function (target)
        import("core.project.config")
        import("core.base.option")

        local resdir = "res"
        os.cp(path.join(resdir, "**"), target:targetdir())
    end)
