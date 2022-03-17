/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2022, Maxar Technologies, Inc.
 *
 * NITRO is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include "nitf/UnitTests.hpp"

#include <assert.h>

#include <std/filesystem>
#include <std/optional>
#include <import/sys.h>

namespace fs = std::filesystem;

static const sys::OS os;
static std::string Configuration() // "Configuration" is typically "Debug" or "Release"
{
	return os.getSpecialEnv("Configuration");
}
static std::string Platform()
{
	return os.getSpecialEnv("Platform");
}

static const fs::path& getCurrentExecutable()
{
	static const auto exec = fs::absolute(os.getCurrentExecutable());
	return exec;
}
static const fs::path& current_path()
{
	static const auto cwd = fs::absolute(fs::current_path());
	return cwd;
}


// https://stackoverflow.com/questions/13794130/visual-studio-how-to-check-used-c-platform-toolset-programmatically
static std::string PlatformToolset()
{
	// https://docs.microsoft.com/en-us/cpp/build/how-to-modify-the-target-framework-and-platform-toolset?view=msvc-160
#ifdef _WIN32
#if _MSC_FULL_VER >= 190000000
	return "v142";
#else
#error "Don't know $(PlatformToolset) value.'"
#endif
#else 
	// Linux
	return "";
#endif
}

static fs::path make_waf_install(const fs::path& p)
{
	// just "install" on Linux; install-Debug-x64.v142 on Windows
#ifdef _WIN32
	const auto configuration_and_platform = Configuration() + "-" + Platform() + "." + PlatformToolset();
	return p / ("install-" + configuration_and_platform);
#else
	// Linux
	return p / "install";
#endif
}

static fs::path make_cmake_install(const fs::path& p)
{
	auto out = p;
	fs::path configuration_and_platform;
	fs::path build;
	while (out.stem() != "out")
	{
		configuration_and_platform = build; // "...\out\build\x64-Debug"
		build = out; // "...\out\build"
		out = out.parent_path(); // "...\out"
	}

	const auto install = out / "install"; // "...\out\install"
	return install / configuration_and_platform.stem(); // "...\out\install\x64-Debug"
}

static std::optional<fs::path> findRoot(const fs::path& p)
{
	if (is_regular_file(p / "LICENSE") && is_regular_file(p / "README.md") && is_regular_file(p / "CMakeLists.txt"))
	{
		return p;
	}
	return p.parent_path() == p ? std::optional<fs::path>() : findRoot(p.parent_path());
}
static const fs::path& findRoot(fs::path& exec_root, fs::path& cwd_root)
{
	static const auto exec_root_ = findRoot(getCurrentExecutable());
	if (exec_root_.has_value())
	{
		exec_root = exec_root_.value();
		return exec_root;
	}

	static const auto cwd_root_ = findRoot(current_path());
	cwd_root = cwd_root_.value();
	return cwd_root;
}
static fs::path findRoot()
{
	fs::path exec_root, cwd_root;
	return findRoot(exec_root, cwd_root);
}

static std::string makeRelative(const fs::path& path, const fs::path& root)
{
	// remove the "root" part from "path"
	std::string relative = path.string();
	str::replaceAll(relative, root.string(), "");
	return relative;
}
static std::string relativeRoot()
{
	fs::path exec_root, cwd_root;
	findRoot(exec_root, cwd_root);

	if (!exec_root.empty())
	{
		return makeRelative(getCurrentExecutable(), exec_root);
	}

	assert(!cwd_root.empty());
	return makeRelative(current_path(), cwd_root);
}

static bool is_cmake_build()
{
	const auto retlativeRoot = relativeRoot();
	return str::starts_with(retlativeRoot, "/out") || str::starts_with(retlativeRoot, "\\out");
}

static fs::path buildDir(const fs::path& path)
{
	static const auto& exec = getCurrentExecutable();
	static const auto exec_filename = exec.filename();

	if (exec_filename == "testhost.exe")
	{
		// Running in Visual Studio on Windows
		return current_path() / path;
	}

	const auto root = findRoot();
	std::string relative_exec = exec.string();
	str::replaceAll(relative_exec, root.string(), "");

	auto extension = exec.extension().string();
	str::upper(extension);
	if (extension == ".EXE")
	{
		// stand-alone executable on Windows (ends in .EXE)
		const auto install = is_cmake_build() ? make_cmake_install(exec) : make_waf_install(root);		
		return install / path;
	}

	//fprintf(stderr, "cwd = %s\n", cwd.c_str());
	//fprintf(stderr, "exec = %s\n", exec.c_str());

	fs::path install;
	if (is_cmake_build())
	{
		install = root / "install" / (Platform() + "-" + Configuration()); // e.g., "x64-Debug"
	}
	else
	{
		install = make_waf_install(root);
	}

	return install / path;
}

std::string nitf::Test::buildPluginsDir()
{
	const auto plugins = buildDir(fs::path("share") / "nitf" / "plugins");
	return plugins.string();
}

static const fs::path argv0;
fs::path nitf::Test::buildFileDir(const fs::path& relativePath)
{
	const auto root = findRoot();
	return root / relativePath;
}
