#!/usr/bin/perl
use strict;
use warnings;
use Cwd;

# Gitリポジトリがあるディレクトリと、コピー先
my $repo_dir = "/home/AC130/robo";
my $copy_to  = "/home/AC130/RasPike-ART";
#my $copy_to  = "/home/AC130/tmptmp";

# 現在のディレクトリを保存
my $original_dir = getcwd();

# Gitディレクトリに移動
chdir $repo_dir or die "ディレクトリに移動できません: $!";

# Git pull
die "このディレクトリはGitリポジトリではありません: $repo_dir\n" unless -d ".git";
system("git pull") == 0 or die "git pull に失敗しました";

# pull した内容をコピー先に反映（上書きコピー）
system("cp -r ./sdk \"$copy_to\"") == 0 or die "cp -r に失敗しました: $!";

# 元のディレクトリに戻る
chdir $original_dir or die "元のディレクトリに戻れません: $!";

print "pullしてコピー完了しました。\n";
