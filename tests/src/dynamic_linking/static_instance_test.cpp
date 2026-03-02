#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #define NOGDI
    #define NOMINMAX
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

#include <gtest/gtest.h>

#include <slog/fmt/pattern_formatter.hpp>
#include <slog/slog.hpp>


// The functions exported by the DLL plugin
typedef bool (*VerifyRegistryFn)(slog::LogLevel);
typedef bool (*VerifyFormatterFn)();

// A dummy format flag just for testing
void custom_shared_flag(const slog::fmt::FormatContext&, std::string& dest)
{
    dest.append("[SHARED]");
}

TEST(DynamicLinkingTest, SharedStaticInstances)
{
    // Modify Registry
    SLOG_REGISTRY.set_log_level(slog::LogLevel::WARNING);

    // Modify PatternFormatter Dispatch Table
    slog::fmt::PatternFormatter::register_flag('X', custom_shared_flag);

    // Load the Plugin DLL
#if defined(_WIN32)
    HMODULE handle = LoadLibraryA(TEST_PLUGIN_PATH);
    ASSERT_NE(handle, nullptr) << "Failed to load " << TEST_PLUGIN_PATH;

    auto verify_registry = reinterpret_cast<VerifyRegistryFn>(GetProcAddress(handle, "verify_registry_instance_test"));
    auto verify_formatter = reinterpret_cast<VerifyFormatterFn>(GetProcAddress(handle, "verify_formatter_instance_test"));
#else
    void* handle = dlopen(TEST_PLUGIN_PATH, RTLD_LAZY);
    ASSERT_NE(handle, nullptr) << "Failed to load " << TEST_PLUGIN_PATH << ": " << dlerror();

    auto verify_registry = reinterpret_cast<VerifyRegistryFn>(dlsym(handle, "verify_registry_instance_test"));
    auto verify_formatter = reinterpret_cast<VerifyFormatterFn>(dlsym(handle, "verify_formatter_instance_test"));
#endif

    ASSERT_NE(verify_registry, nullptr) << "Failed to find verify_registry_instance_test definition";
    ASSERT_NE(verify_formatter, nullptr) << "Failed to find verify_formatter_instance_test definition";

    bool status{false};

    // Registry Check
    status = verify_registry(slog::LogLevel::WARNING);
    EXPECT_TRUE(status) << "DLL plugin sees a different Registry instance!";

    // Dispatch Table Check
    status = verify_formatter();
    EXPECT_TRUE(status) << "DLL plugin sees a different PatternFormatter _dispatch_table!";

#if defined(_WIN32)
    FreeLibrary(handle);
#else
    dlclose(handle);
#endif
}
