#!/usr/bin/perl
use strict;
use warnings;
use Cwd;

# コピー元とコピー先
my $src_dir  = "/home/AC130/RasPike-ART/sdk";
my $dest_dir = "/home/AC130/robo";

# 現在のディレクトリを保存
my $original_dir = getcwd();

# ディレクトリをコピー（再帰的）
system("cp -r \"$src_dir\" \"$dest_dir\"") == 0 or die "cp -r に失敗しました: $!";

# コピー先に移動
chdir $dest_dir or die "ディレクトリに移動できません: $!";

# コマンドライン引数からコミットメッセージを取得
my $commit_message = join(" ", @ARGV);
die "コミットメッセージを指定してください。\n" unless $commit_message;

# Git操作
system("git add .") == 0 or die "git add に失敗しました";
system("git commit -m \"$commit_message\"") == 0 or die "git commit に失敗しました";
#system("git push") == 0 or die "git push に失敗しました";
system("git push --force") == 0 or die "git push に失敗しました";

# 元のディレクトリに戻る
chdir $original_dir or die "元のディレクトリに戻れません: $!";

print "完了しました。\n";
