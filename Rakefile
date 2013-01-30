$:.unshift '/home/benjo/github/rake-builder/lib'

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

Rake::Builder.new do |builder|
  builder.task_namespace = :test
  builder.target = 'output/host/test/hp03stest'
  builder.source_search_paths = ['tests/**', 'mocks']
  builder.objects_path = 'output/host/test/obj'
  builder.include_paths = ['include', 'mocks', "#{ENV['CPPUTEST_HOME']}/include"]
  builder.library_dependencies = ['hp03s', 'CppUTest']
  builder.library_paths = ['output/host/lib', "#{ENV['CPPUTEST_HOME']}/lib"]
  builder.default_task = :run
end
