//---------------------------------------------------------------------------------------------------------------------------------
// Copyright (C) 2011 Cristian Adam
//---------------------------------------------------------------------------------------------------------------------------------
var ForReading = 1, ForWriting = 2;

function ReplaceStringInFile(fileName, rgExp, replaceText)
{
  var fso = new ActiveXObject("Scripting.FileSystemObject");
  var file = fso.OpenTextFile(fileName, ForReading);
  var content = file.ReadAll();
  file.Close();
  
  content = content.replace(rgExp, replaceText);

  try
  {
    fso.DeleteFile(fileName + ".bak", true);
  }
  catch (e)
  {
  }
  fso.MoveFile(fileName, fileName + ".bak");

  file = fso.OpenTextFile(fileName, ForWriting, true);
  file.Write(content);
  file.Close();
}


function main()
{
  if (WScript.Arguments.length != 3)
  {
    WScript.Echo("Usage: replace_string.js file rgExp replaceText");
    WScript.Quit(1);
  }
  
  ReplaceStringInFile(WScript.Arguments.Item(0), WScript.Arguments.Item(1), WScript.Arguments.Item(2));
}

main();