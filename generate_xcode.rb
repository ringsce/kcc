# generate_xcode.rb

require 'xcodeproj'

# --- Configuration ---
project_name = 'KCC'
project_path = "#{project_name}.xcodeproj"
source_dir = 'src'
include_dir = 'include'
# -------------------

puts "Creating Xcode project at #{project_path}..."

# Create a new Xcode project
project = Xcodeproj::Project.new(project_path)

# Add a target for the main executable
target = project.new_target(:command_line_tool, project_name, :osx)

# Find all .c source files and .h header files
source_files = Dir.glob("#{source_dir}/**/*.c")
header_files = Dir.glob("#{include_dir}/**/*.h")

# Create file groups in the Xcode project navigator
sources_group = project.main_group.find_subpath('Sources', true)
headers_group = project.main_group.find_subpath('Headers', true)

# Add source files to the project and the target
source_files.each do |file_path|
  file = sources_group.new_file(file_path)
  target.add_file_references([file])
end

# Add header files to the project
header_files.each do |file_path|
  headers_group.new_file(file_path)
end

# Set build settings for the target
target.build_configurations.each do |config|
  config.build_settings['PRODUCT_NAME'] = project_name
  # Tell Xcode where to find the header files
  config.build_settings['HEADER_SEARCH_PATHS'] = "$(SRCROOT)/#{include_dir}"
end

# Save the project to disk
project.save

puts "✅ Xcode project generated successfully!"
puts "You can now open '#{project_path}' in Xcode."
