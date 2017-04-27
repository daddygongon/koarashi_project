# -*- coding: utf-8 -*-
# this is supplied by hiki -i 
# hikiutils are synchronizing system between local dir and hiki
# and also supplied hiki_dir with latex_dir
#    cc by Shigeto R. Nishitani, 2017
require 'systemu'
require 'tempfile'
require 'colorize'
require 'coderay'

begin
  $hiki_dir = File.readlines('./.hikirc')[0].chomp
rescue => e
  puts e
  exit
end
dirnames=Dir.pwd.split('/')
$basename = (dirnames[-1]=='hikis')? dirnames[-2] : dirnames[-1]
$latex_dir= 'latex_dir'
$section_layer = {}
$target_dir = ARGV[1] || '.'

task :default do
  system 'rake -T'
end

desc "Githubのdirをsafariでopen"
task :github do
  status, stdout, stderr = systemu %q( git remote -v |grep origin )
  github_dir=stdout.match(/(.+):(.+) \(push\)/)[2]
  puts github_dir.blue
  system "open https://github.com/#{github_dir}"
end

desc "hikiシステムにあるゴミファイルを掃除する"
task :reset_hiki do
  status, stdout, stderr = systemu "hiki_optparse -l #{$basename}"
  print stdout
  files=[]
  stdout.split("\n").each{|line|
    files << line.split(/\s+/)[-1]
  }
  r_files = files.reverse[0..-4]
  r_files.each{|file|
    print "remove #{file}[ynqlA]? ".red
    input=STDIN.gets.chomp
    case input
    when 'y'
      p command="hiki_optparse --remove #{file}"
      system command
    when 'n'
    when 'l'
      p command="hiki_optparse -l #{$basename}"
      system command
    when 'q'
      exit
    when 'A'
      print "\nAre you sure[Yn]? ".magenta
      input2 = STDIN.gets.chomp
      case input2
        when 'Y'
        r_files.each{|file| system "hiki_optparse --remove #{file}"}
        p target = File.join($hiki_dir,'cache','attach',$basename)
        exit
        when 'n'
        exit
      end
    end
  }
end

desc "latex_dirのゴミを掃除"
task :reset_latex_dir do
  system "mv latex_dir/head.tex ."
  system "mv latex_dir/jlisting.sty ."
  system "rm -rf latex_dir"
  system "mkdir latex_dir"
  system "mv head.tex latex_dir/"
  system "mv jlisting.sty latex_dir/"
  system "rake latex_all"
end

desc "toc.hikiの作成"
task :mk_toc do
  p target = $basename+'.toc'
  cont=""
  File.readlines(File.join('latex_dir',target)).each{|line|
    if m=line.match(/\\contentsline \{(.+)\}\{\\numberline \{([\d|\.]+)\}(.+)\}\{\d+\}/)
      layer,sec_no,title=m[1],m[2],m[3]
      d = case m[1]
          when 'section'; 1
          when 'subsection'; 2
          when 'subsubsection'; 3
          else; 4
          end
      layer = "!"*d
      title.gsub!('\_','_')
      cont <<  "#{layer}#{sec_no}:#{title}\n"
    end
  }
  print cont
  File.open("toc.hiki",'w'){|file| file.print cont}
end

desc "FILE.hikiあるいはhikiファイルすべてを最新状態に更新"
task :touch do
  name=ARGV[1]||'.'
  if File.exist?(File.join(name,$basename+'.hiki')) then
    $target_dir = name
  else
    p file = File.basename(name)
  end

  if file
    file_split=file.split(".")
    p target = $basename==file_split[0] ? $basename : $basename+"_"+file_split[0]
    system "hiki_optparse -u #{target}"
  else
    p target = File.join($hiki_dir,'cache','attach',$basename)
    system "touch #{target}/*"
    Dir.entries($target_dir).each{|file|
      puts file.red
      file_split=file.split(".")
      if file_split[1]=='hiki'
        p target = $basename==file_split[0] ? $basename : $basename+"_"+file_split[0]
        p command= "hiki_optparse -u #{target}"
        system command
      end
    }
  end
  exit
end

desc "FILE.hikiあるいはhikiファイルをedtiorで開く"
task :open do
  name=ARGV[1]||'.'
  if File.exist?(File.join(name,$basename+'.hiki')) then
    $target_dir = name
  else
    p file = name
  end
  if file
    file += '.hiki' if file.split(".")[1]==nil
    system "open -a mi #{file}"
  else
    Dir.entries($target_dir).each{|file|
      file_split=file.split(".")
      if file_split[1]=='hiki'
        p target = File.join($target_dir,file)
        system "open -a mi #{target}"
      end
    }
  end
end

desc "FILE1をlatexに変換"
task :latex => [:latex_base] do
  exit
end

desc "FILE1をwrap formatでlatexに変換"
task :latex_wrap => [:latex_base, :change_wrap] do
  exit
end

def latex_loop(entries,opts={})
  check_bounding_box
  main_file=""
  entries.each{|file|
    options={:latex_all=>true}
    file_basename= File.basename(file)
    next unless file_basename.split('.')[1]=='hiki'
    next if file_basename=='toc.hiki'
    next if file_basename.include?('.hikirc')
    puts file.red
    f_name =File.basename(file,'.hiki')
    if opts[:main]
      if f_name==$basename
        options[:latex]="--listings --head latex_dir/head.tex -p "
        options[:main]=true
        main_file=f_name
        convert_to_latex(file, options)
      end
    else
      if f_name!=$basename
        p level = $section_layer[f_name] || 1
        options[:latex]="-l #{level} --listings -b "
        convert_to_latex(file, options)
      end
    end
  }
  return main_file
end

def check_section_layers(lines)
  p reg_exp = Regexp.new("#{$basename}_(.+)")
  lines.each{|line|
    p line
    if m=line.match(/(!+)\[\[(.+)\]\]/)
      p count = m[1].count('!')
      p file_name=m[2].match(reg_exp)[1]
      $section_layer[file_name]=count
    end
  }
  p $section_layer
end

def check_bounding_box
#  $bounding_box="0 0 442 432"
#  $bounding_box=" 0 0 737 553" # for 60% converted
  $fig_width="12" #for wrap figure
  $bounding_box=" 0 0 937 753" # for original keynote output
#  $fig_width="12" # for full size width
end

def get_target_entries
  p entries=Dir.entries($target_dir)
  entries.map!{|file| File.join($target_dir, file)}
  puts entries.to_s.red
  return entries
end

desc "すべてのDIR/hikiファイルをlatex変換"
task :latex_all do
  entries = get_target_entries
  main_file = latex_loop(entries,opts={:main=>true})
  latex_loop(entries)
  cont=""
  p reg_exp = Regexp.new("#{$basename}_(.+)")
  toc = false
  File.readlines(File.join($latex_dir,"#{main_file}.tex")).each{|line|
#    line, toc = "", true if line.match(/\\tableofcontents/) and toc == true #only one
    if line.match(/\\tableofcontents/)
      if toc == false #only one
        toc = true 
      else
        line = ''
      end
    end
    line ="\\usepackage{url}\n\\usepackage{listings,jlisting}\n" if line.match(/\\usepackage{listings}/)
    if line.match(/section{(.+)}/)
      if m=line.match(/\\verb\|(.+)\((.+)\)\|/)
        p m
        p m_t=m[2].match(reg_exp)[1]
        line = "\\include{#{m_t}}\n"
      elsif m=line.match(/section{\\verb\|(.+)\|}/)
        p m
        p m_t=m[1].match(reg_exp)[1]
        line = "\\include{#{m_t}}\n"
      else
#        line = ""
      end
    end
    cont << line
  }
  File.open(File.join($latex_dir,"#{main_file}.tex"),'w'){|file| file.print cont}
  system "open latex_dir/#{main_file}.tex"
  exit
end

task :latex_base do
  check_bounding_box
  Dir.mkdir($latex_dir) unless Dir.exist?($latex_dir)
  convert_to_latex(ARGV[1])
  system("open #{$save_name}")
end

def convert_to_latex(file_name, options={})
  p file_name = file_name.include?('.hiki')? file_name : file_name+'.hiki'
  p tex_name = File.basename(file_name,'.hiki')+'.tex'
  p $save_name = ARGV[2] || File.join($latex_dir,tex_name)
  pre_name = File.join($latex_dir,'pre.tex')
  p pre_command = File.exist?(pre_name) ? '--pre '+pre_name : nil
  p pre_command = ''

  lines = File.readlines(file_name)
  check_section_layers(lines) if options[:main]
  cont = ""
  lines.each_with_index{|line,i|
    if m=line.match(/\{\{attach_view\((.*),(.*)\)\}\}/)
      p line="\{\{attach_view\(#{m[1]}\)\}\}\n"
    elsif options[:latex_all] and i<5
      line="" if line.match(/^!title:|^!author:|^!date:/)
    end
    cont << line
  }
  tf1,tf2='tmp1.txt','tmp2.txt'
  File.open(tf1,'w'){|file| file.print cont}
  options[:latex]="--head latex_dir/head.tex -p "
  system "hiki2latex #{pre_command} #{options[:latex]} #{tf1} > #{tf2}"
  lines = File.readlines(tf2)
  system "rm #{tf1} #{tf2}"
  in_bib,in_abst,cont=false,false,""
  lines.each{|line|
    line=latex_cite_ref(line)
    if m=line.match(/\\caption{\s*\((.+?)\)(.+)}/)
      p m
      puts line = "\\caption{#{m[2]}}\n\\label{#{m[1]}}\n"
    end
    if m=line.match(/\\section{reference:}(.+)/) and in_bib==false
      in_bib=true
      line = "\\begin{thebibliography}{99}\n"
      puts line.blue
    end
    if m=line.match(/\\end{description}/) and in_bib==true
      in_bib=false
      line = "\\end{thebibliography}\n"
    end
    if m=line.match(/\\item\[(.+?)\](.+)/) and in_bib==true
      tmp = m[2].gsub!(/\\verb\|(.*?)\|/){ "\\url{#{$1}}" } || m[2]
      tmp = m[2].gsub!(/\\verb\|(.*?)\|/){ "" } || m[2]
      line ="\\bibitem{#{m[1]}} #{tmp}"
      puts line.blue
    end
    if m=line.match(/\\section{abstract:}/) and in_abst==false
      in_abst=true
      line = "\\begin{abstract}\n"
      puts line.blue
    end
    if m=line.match(/\\section/) and in_abst==true
      in_abst=false
      line = "\\end{abstract}\n\n#{line}\n"
    end
    if m=line.match(/\\includegraphics\[width=6cm\]\{(.+)\}/)
      p line="\\includegraphics\[width=#{$fig_width}cm,bb=#{$bounding_box}\]\{../figs/#{m[1]}\}\n"
      cont << line
    else
      cont << line
    end
  }
  File.open($save_name,'w'){|file| file.print cont }
end

def latex_cite_ref(line)
  r_pair=[/\\verb\|{{cite\((.+?)\)}}\|/,/\\verb\|{{ref\((.+?)\)}}\|/]
  while m=line.match(r_pair[0])
    line.sub!(r_pair[0],"\\cite\{#{m[1]}\}")
  end
  while m=line.match(r_pair[1])
    line.sub!(r_pair[1],"\\ref\{#{m[1]}\}")
  end
#  puts line.chomp.blue
  return line
end

task :change_wrap do
  lines = File.readlines($save_name)

  cont = ""
  lines.each{|line|
    if line.include?('\begin{figure}[htbp]\begin{center}')
      p line
      cont << '\begin{wrapfigure}{r}{8cm}'+"\n"
      cont << '\vspace{-2\baselineskip}'+"\n"
      cont << '\begin{center}'+"\n"
    elsif line.include?('\label{default}\end{center}\end{figure}')
      p line
      cont << '\end{center}'+"\n"
      cont << '\vspace{2\baselineskip}'+"\n"
      cont << '\end{wrapfigure}'+"\n"
    else
      cont << line
    end
  }
  File.open($save_name,'w'){|file| file.print cont }

  system("open #{$save_name}")
  exit
end


desc "increment fig NUBERS in FILE"
task :increment do
  number=ARGV[1]
  file = ARGV[2]
  dir = ARGV[3] || nil
  lines = File.readlines(file)
  cont = ""
  lines.each{|line|
    if m=line.match(/\{\{attach_view\((\w+|_).(\d+).jpeg\)\}\}/)
      new_num=sprintf("%03d",m[2].to_i+number.to_i)
      line="\{\{attach_view\(#{m[1]}.#{new_num}.jpeg,#{dir}\)\}\}\n"
      cont << line
    else
      cont << line
    end
  }
  print cont
  exit
end

desc "numbering figs from the NUBER in FILE"
task :number do
  number=ARGV[1].to_i
  file = ARGV[2]
  dir = ARGV[3] || nil
  lines = File.readlines(file)
  cont = ""
  lines.each{|line|
    if m=line.match(/\{\{attach_view\((\w+|_).(\d+).jpeg,(\w+)\)\}\}/)
      new_num=sprintf("%03d",number)
      line="\{\{attach_view\(#{m[1]}.#{new_num}.jpeg,#{m[3]}\)\}\}\n"
      cont << line
      number += 1
    else
      cont << line
    end
  }
  print cont
  exit
end

desc "convert fig size SCALE TARGET_DIR"
task :convert do
  scale = ARGV[1]
  target_dir=ARGV[2]
  Dir.entries(target_dir)[2..-1].each{|file|
    p file
    source = File.join(target_dir,file)
    target = File.join('figs',file)
    p command = "convert #{source} -resize #{scale}\% #{target}"
    system command
  }
  exit
end

desc "hikiの同期"
task :sync => [:check_previous,:sync0]


desc "hikiの強制同期"
task :force_sync => [:sync0]

def hiki_cite_ref(file)
  tf1 = 'tmp1.txt'
  lines = File.readlines(file)
  inside_pre_form,cont = false,""
  lines.each{|line|
    inside_pre_form=true if line.match(/^<<</) and !inside_pre_form
    inside_pre_form=false if line.match(/^>>>/) and inside_pre_form
    tr_pair=[/\{\{cite\((.+?)\)\}\}/,/\{\{ref\((.+)\)\}\}/]
    while m=line.match(tr_pair[0]) and !inside_pre_form
      puts line.sub!(tr_pair[0], "\[[[#{m[1]}|#{$basename}_references]]\]").chomp.green
    end
    while m=line.match(tr_pair[1]) and !inside_pre_form
      puts line.sub!(tr_pair[1],"#{m[1]}").chomp.green
    end
    cont << line
  }
  File.open(tf1,'w'){|file| file.print cont }
  return tf1
end

task :sync0 do
  entries=get_target_entries
  entries.each{|file| #cp *.hiki
    file_basename= File.basename(file)
    next unless file_basename.split('.')[1]=='hiki'
    next if file_basename.include?('.hikirc')
    f_name =File.basename(file,'.hiki')
    puts f_name.red
    name = (f_name==$basename)? f_name : $basename+'_'+f_name
    p target = File.join($hiki_dir,'text',name)
    source = hiki_cite_ref(file)
    FileUtils.cp(source,target,:verbose=>true)
  }

  source_dirs=['./figs','./refs']
  source_dirs.each{|source_dir|
    next unless Dir.exist?(source_dir)
    p entries=Dir.entries(source_dir) #cp files in figs
    p name = $basename+'_'+File.basename(source_dir)
    p target = File.join($hiki_dir,'cache','attach',name)
    FileUtils.mkdir_p(target,:verbose=>true) unless File.exists?(target)
    entries[2..-1].each{|file|
      next if FileTest.directory?(File.join(source_dir,file))
      source = File.join(source_dir,file)
#      FileUtils.cp(source,target,:verbose=>true)
      FileUtils.cp(source,target)
    }
  }
  File.open('./.hikirc','w'){|file|
    status, stdout, stderr =systemu "hiki_optparse -l #{$basename}*"
    file.print($hiki_dir+"\n")
    stdout.split("\n")[3..-1].each{|line|
      file.print line+"\n"
    }
  }
  exit
end

desc "For hiki Errno::ENOENT, Errno::EACCES"
task :chenv do
  p user = ENV['USER']
  system("sudo chmod -R a+w #{$hiki_dir}")
#  system("sudo chown -R #{user} #{$hiki_dir}")
end

desc "self copy to hikiutils template directory"
task :self_copy do
  p cp_files=[[File.join(Dir.pwd,'Rakefile'),'Rakefile_hiki_sync'],
              [File.join(Dir.pwd,'Rakefile'),'Rakefile_hiki_sync_new'],
            [File.join(ENV['HOME'],'.my_help','hiki_help.yml'),'hiki_help.yml']]
  cp_files.each{|files|
    p source = files[0]
    p target = File.join('/Users/bob/Github/hikiutils/lib/templates/',files[1])
    command = "diff #{source} #{target}"
    status, stdout, stderr=systemu command
    if stdout!=''
      puts command.red
      puts CodeRay.scan(stdout, :Diff).term
      print " Are you sure to save #{source}? [Yn]  ".red
      FileUtils.cp(source,target,:verbose=>true) if STDIN.gets.chomp=='Y'
    end
  }
end

task :check_previous do
  current={}
  print "current hiki dir\n"
  status, stdout, stderr = systemu("hiki_optparse -l #{$basename}*")
  stdout.split("\n")[3..-1].each{|line|
    p line
    file=   line.split(/\s+/)[-1]
    current[file]=line
  }
  previous={}
  print "previous sync\n"
  File.readlines('.hikirc')[1..-1].each{|line|
    p line.chomp
    file=   line.split(/\s+/)[-1]
    previous[file]=line.chomp
  }
  changed = []
  current.each_pair{|key,val|
    if !previous.include?(key) or previous[key]!=val
      changed << val+"\n"
    end
  }
  if changed.size != 0
    print("hiki dirで変更が発生しています．\n")
    changed.each{|line| print line+"\n"} 
    exit
  end
end
