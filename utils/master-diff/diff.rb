#!/usr/bin/env ruby
require 'open3'
require 'fileutils'
require 'set'
require 'time'
require 'yaml'

MASTER = 'master'
FORK   = 'release/1.6.0'

SCRIPT_DIR    = File.expand_path(__dir__)
REPO_ROOT     = File.expand_path('../..', SCRIPT_DIR)
OUTPUT_DIR    = File.join(SCRIPT_DIR, 'output')
EXCLUDED_FILE = File.join(SCRIPT_DIR, 'master-only.yaml')


def run_git(args)
  out, err, status = Open3.capture3('git', *args, chdir: REPO_ROOT)
  unless status.success?
    $stderr.puts "git error: #{err}"
    exit 1
  end
  out
end


def parse_cherry(output)
  output.lines.filter_map do |line|
    next if line.strip.empty?
    sign    = line[0]
    parts   = line[2..].split(' ', 2)
    sha     = parts[0]
    subject = parts[1]&.strip || ''
    [sign, sha, subject]
  end
end


def get_patch_id_map(commit_hashes)
  return {} if commit_hashes.empty?

  hashes_input = (commit_hashes.join("\n") + "\n").b

  # Keep diff as binary — repo may contain non-UTF-8 content
  diff_out, = Open3.capture2(
    'git', 'diff-tree', '-p', '--stdin',
    stdin_data: hashes_input, chdir: REPO_ROOT, binmode: true
  )

  patch_out, = Open3.capture2(
    'git', 'patch-id',
    stdin_data: diff_out, chdir: REPO_ROOT, binmode: true
  )

  result = {}
  patch_out.force_encoding('utf-8').scrub.each_line do |line|
    parts = line.split
    next unless parts.length == 2
    patch_id, sha = parts
    result[patch_id] = sha
  end
  result
end


def load_exclusions(path)
  excluded_shas   = Set.new
  excluded_titles = Set.new
  return [excluded_shas, excluded_titles] unless File.exist?(path)

  entries = YAML.safe_load(File.read(path))
  return [excluded_shas, excluded_titles] unless entries.is_a?(Array)

  entries.each do |entry|
    next unless entry.is_a?(Hash)
    excluded_shas   << entry['commit'].to_s if entry.key?('commit')
    excluded_titles << entry['title'].to_s  if entry.key?('title')
  end
  [excluded_shas, excluded_titles]
end


def write_file(path, content)
  File.write(path, content)
  puts "Wrote #{path}"
end


def main
  FileUtils.mkdir_p(OUTPUT_DIR)

  puts "Running: git cherry -v #{FORK} #{MASTER} ..."
  master_cherry    = parse_cherry(run_git(['cherry', '-v', FORK, MASTER]))
  master_only      = master_cherry.select { |s, _, _| s == '+' }.map { |_, sha, subj| [sha, subj] }
  picked_from_master = master_cherry.select { |s, _, _| s == '-' }.map { |_, sha, subj| [sha, subj] }

  puts "Running: git cherry -v #{MASTER} #{FORK} ..."
  fork_cherry = parse_cherry(run_git(['cherry', '-v', MASTER, FORK]))
  fork_all    = fork_cherry.select { |s, _, _| s == '+' }.map { |_, sha, subj| [sha, subj] }

  fork_only = fork_all

  excluded_shas, excluded_titles = load_exclusions(EXCLUDED_FILE)
  master_only_pending  = []
  master_only_excluded = []
  master_only.each do |sha, subj|
    if excluded_shas.include?(sha) || excluded_titles.include?(subj)
      master_only_excluded << [sha, subj]
    else
      master_only_pending << [sha, subj]
    end
  end

  puts 'Computing patch-ids for cherry-picked commit matching ...'

  picked_master_shas = picked_from_master.map { |sha, _| sha }
  master_patch_ids   = get_patch_id_map(picked_master_shas)  # {patch_id => master_sha}

  fork_commit_shas = run_git(['log', '--format=%H', FORK, "^#{MASTER}"]).split
  fork_patch_ids   = get_patch_id_map(fork_commit_shas)      # {patch_id => fork_sha}

  master_to_fork = {}
  master_patch_ids.each do |pid, msha|
    master_to_fork[msha] = fork_patch_ids[pid] if fork_patch_ids.key?(pid)
  end

  both_entries = []  # [master_sha, fork_sha, subject]
  unmatched    = []  # [master_sha, subject]
  picked_from_master.each do |sha, subj|
    fork_sha = master_to_fork[sha]
    if fork_sha
      both_entries << [sha, fork_sha, subj]
    else
      unmatched << [sha, subj]
    end
  end

  # Write summary.txt
  sep = '=' * 72
  lines = [
    "Master:       #{MASTER}",
    "Fork:         #{FORK}",
    '',
    'Counts:',
    "  Master-only, pending (not cherry-picked):  #{master_only_pending.length}",
    "  Master-only, excluded (won't backport):    #{master_only_excluded.length}",
    "  Cherry-picked to both:                    #{both_entries.length}",
    "  Unmatched picked (no patch-id match):     #{unmatched.length}",
    "  Fork-only:                                #{fork_only.length}",
    '',
    sep,
    "MASTER-ONLY PENDING — not yet cherry-picked to fork (#{master_only_pending.length} commits)",
    sep,
  ] + master_only_pending.map { |sha, subj| "  + #{sha}  #{subj}" } + [
    '',
    sep,
    "MASTER-ONLY EXCLUDED — deliberately not backported (#{master_only_excluded.length} commits)",
    sep,
  ] + master_only_excluded.map { |sha, subj| "  + #{sha}  #{subj}" } + [
    '',
    sep,
    "CHERRY-PICKED TO BOTH (#{both_entries.length} commits)",
    sep,
  ] + both_entries.map { |msha, fsha, subj| "  master: #{msha}  fork: #{fsha}  #{subj}" }

  unless unmatched.empty?
    lines += [
      '',
      sep,
      "UNMATCHED PICKED — no patch-id match found (#{unmatched.length} commits)",
      sep,
    ] + unmatched.map { |sha, subj| "  - #{sha}  #{subj}" }
  end

  lines += [
    '',
    sep,
    "FORK-ONLY (#{fork_only.length} commits)",
    sep,
  ] + fork_only.map { |sha, subj| "  + #{sha}  #{subj}" }

  write_file(File.join(OUTPUT_DIR, 'summary.txt'), lines.join("\n") + "\n")

  # Write changes-master.yaml
  write_file(
    File.join(OUTPUT_DIR, 'changes-master.yaml'),
    master_only_pending.map { |sha, subj| "- commit: #{sha}  # #{subj}" }.join("\n") + "\n"
  )

  # Write changes-fork.yaml
  write_file(
    File.join(OUTPUT_DIR, 'changes-fork.yaml'),
    fork_only.map { |sha, subj| "- commit: #{sha}  # #{subj}" }.join("\n") + "\n"
  )

  # Write changes-fork-excluded.yaml
  write_file(
    File.join(OUTPUT_DIR, 'changes-fork-excluded.yaml'),
    master_only_excluded.map { |sha, subj| "- commit: #{sha}  # #{subj}" }.join("\n") + "\n"
  )

  # Write changes-both.yaml
  both_yaml = both_entries.flat_map do |msha, fsha, subj|
    ["- master: #{msha}", "  fork: #{fsha}  # #{subj}"]
  end.join("\n") + "\n"
  write_file(File.join(OUTPUT_DIR, 'changes-both.yaml'), both_yaml)

  puts "\nDone. Output in #{OUTPUT_DIR}"
end

main
