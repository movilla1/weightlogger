String render(const String text, String title, char *selected, bool isFile, bool partial=true) {
  String tpl;
  String content;
  if (isFile == true) {
    content = getFromSpiffs(text);
  } else {
    content = text;
  }
  if (partial) {
    tpl = getFromSpiffs("/_template.html");
    tpl = replace_tag(tpl, "[[title]]", title);
    tpl = replace_tag(tpl, "[[content]]", content);
    if (selected[0]==1) {
      tpl = replace_tag(tpl,"[[active_dash]]", "active");
    } else {
      tpl = replace_tag(tpl,"[[active_dash]]", "");
    }
    if (selected[1]==1) {
      tpl = replace_tag(tpl,"[[active_server]]", "active");
    } else {
      tpl = replace_tag(tpl,"[[active_server]]", "");
    }
    if (selected[2]==1) {
      tpl = replace_tag(tpl,"[[active_tags]]", "active");
    } else {
      tpl = replace_tag(tpl,"[[active_tags]]", "");
    }
  }
  return tpl;
}

String replace_tag(String source, String tag, String content ) {
  String result = source;
  result.replace(tag, content);
  return result;
}
