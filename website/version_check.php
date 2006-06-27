<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>
<head>
  <title>oggcodecs Version Check</title>
  <meta name="description" content="Directshow Ogg demuxer/muxer with vorbis, speex, FLAC and theora decoders and encoders.">
  <meta name="keywords" content="directshow, theora, vorbis, speex, ogg, media player, flac, codec, windows, audio, video, wmp, directX, plugin, plug-in, decoder, encoder, demux, mux, demuxer">
</head>

<body bgcolor="#FFFFFF" text="#000000" link="#0000FF" vlink="#DD0000" alink="#FF0000">
<?php
   $x=$_GET['version'];
   $v='0.71.0946';

   if (strcmp($x, '') == 0) {
	echo 'Invalid request';
   } else {
    
     if (strcmp($v, $x) > 0) {
	  echo 'New version ', $v, ' is available';
     } else {
          echo 'No new version available. Current version is ', $v;
     }
   }
?>
	

</body>
</html>