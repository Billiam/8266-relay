#!/usr/bin/env ruby

html_content = File.read File.expand_path("../src/index.html", __dir__)
# Escape percents that don't look like variables
html_content.gsub!(/%[^%\n]*(%|$)/m) do |m|
  m.end_with?('%') ? m : '%' + m
end

h_file = <<~HTML
const char index_html[] PROGMEM = R"=====(
#{html_content}
)=====";
HTML

puts h_file