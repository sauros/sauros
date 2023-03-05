#include "fetch.hpp"
#include <iostream>

#include "indicators.hpp"

#include <git2.h>
#include <sauros/rang.hpp>

namespace mgmt {

namespace {

struct progress_data_s {
  indicators::ProgressBar bar;
};

int fetch_progress(const git_indexer_progress *stats, void *payload) {
  auto pd = static_cast<progress_data_s *>(payload);

  if (stats->received_objects == 0 || pd->bar.is_completed()) {
    return 0;
  }

  if (stats->received_objects == stats->total_objects) {
    pd->bar.mark_as_completed();
    return 0;
  }

  uint8_t percentage_completed =
      ((float)stats->received_objects / (float)stats->total_objects) * 100.00;
  pd->bar.set_progress(percentage_completed);

  return 0;
}

void checkout_progress(const char *path, size_t current, size_t total,
                       void *payload) {}
} // namespace

bool clone_repo(std::string remote_url, std::filesystem::path dir,
                std::string target_name) {

  if (!std::filesystem::is_directory(dir)) {
    std::cout << rang::fg::red << "Error: " << rang::fg::reset << dir
              << " is not a directory" << std::endl;
    return false;
  }

  auto cwd = std::filesystem::current_path();

  std::filesystem::current_path(dir);

  auto temp_repo_dir = dir;
  temp_repo_dir /= target_name;

  bool return_value{false};

  git_libgit2_init();

  progress_data_s d = {indicators::ProgressBar{
      indicators::option::BarWidth{50}, indicators::option::Start{"["},
      indicators::option::Fill{"■"}, indicators::option::Lead{"■"},
      indicators::option::Remainder{"-"}, indicators::option::End{"]"},
      indicators::option::PostfixText{"Cloning target"},
      indicators::option::ForegroundColor{indicators::Color::blue},
      indicators::option::FontStyles{
          std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}}};

  d.bar.set_progress(0);

  git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;

  clone_opts.checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
  clone_opts.checkout_opts.progress_cb = checkout_progress;
  clone_opts.checkout_opts.progress_payload = &d;
  clone_opts.fetch_opts.callbacks.transfer_progress = fetch_progress;
  clone_opts.fetch_opts.callbacks.payload = &d;

  git_repository *repo = NULL;
  int error = git_clone(&repo, remote_url.c_str(),
                        temp_repo_dir.string().c_str(), &clone_opts);

  if (error < 0) {
    const git_error *e = git_error_last();
    std::cout << rang::fg::red << "Error: " << rang::fg::reset << error << "/"
              << e->klass << rang::fg::yellow << " " << e->message << std::endl;
  } else {
    return_value = true;
  }

  git_libgit2_shutdown();

  std::filesystem::current_path(cwd);

  return return_value;
}

bool fetch(std::string remote_url) {

  std::cout << "Fetching " << remote_url << std::endl;

  if (!clone_repo(remote_url, "/tmp", "test_repo")) {
    std::cerr << rang::fg::red << "[FAIL]" << rang::fg::reset
              << "Unable to clone target: " << remote_url << std::endl;
  }

  // CD into the directory that we cloned

  // Ensure that `dino.saur` exists in the root

  // Read everything in and see if we need to build

  return true;
}

} // namespace mgmt