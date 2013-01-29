
require 'rake'
require 'rake/builder'

Rake::Builder.new do |builder|
  builder.target = 'output/host/lib/libhp03s.a'
  builder.source_search_paths = ['src/**.c']
  builder.installable_headers = ['include']
  builder.objects_path = 'output/host/lib/obj'
  builder.compilation_options = ['-Wall',  '-Wextra']
  builder.programming_language = 'c'
end
