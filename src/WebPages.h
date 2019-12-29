const char CONFIG_PAGE[] PROGMEM = R"=====(
<HTML>
 <HEAD>
   <TITLE>Air Quality Monitor v2</TITLE>
 </HEAD>
 <BODY>
  <form action="/settings" method="get">
   %s<br><br>
   %s<br><br>
   %s<br><br>
   <br><br>
   <input type="submit" value="Save" style='width: 150px;'>
   &nbsp;&nbsp;&nbsp;
   <a href="/reboot">
    <button type="button" style='width: 150px;'>Restart</button>
   </a>
  </form>
 </BODY>
</HTML>
)=====";

const char GET_JSON[] PROGMEM = R"=====({
 "temp":%.1f,
 "raw_temp":%.1f,
 "humidity":%.1f,
 "raw_humidity":%.1f,
 "abs_humidity": %.2f,
 "pressure":%.1f,
 "co2":%d
})=====";
