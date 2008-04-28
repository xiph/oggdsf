<?php if (!defined('PmWiki')) exit();

/************************************************************************************
 * FUNCTION DEFAULTS
 ************************************************************************************/

global $barthelme_bodyfontsize;
global $barthelme_bodyfontfamily;
global $barthelme_headerfontfamily;
global $barthelme_postentryalignment;
global $barthelme_wrapperwidth;
global $barthelme_sidebartext;

global $barthelme_sidebar_visible, $EnableMenuBar;

if ($EnableMenuBar != 1) {
	$barthelme_sidebar_visible = "style=\"display: none;\"";
}

$barthelme_sidebartext_visible = FALSE;

/************************************************************************************
 * FUNCTION CALLS
 ************************************************************************************/

function barthelme_bodyfontsize() {
	if (isset($barthelme_bodyfontsize)) {
		print 'body { font-size: ';
		print $barthelme_bodyfontsize;
		print "; }\n";
	}
	else {
		print 'body { font-size: ';
		print '75%';
		print "; }\n";
	}
}

function barthelme_bodyfontfamily() {
	if (isset($barthelme_bodyfontfamily)) {
		print 'body { font-family: ';
		print $barthelme_bodyfontfamily;
		print "; }\n";
	}
	else {
		print 'body { font-family: ';
		print 'arial, helvetica, sans-serif';
		print "; }\n";
	}
}	

function barthelme_headerfontfamily() {
	if (isset($barthelme_headerfontfamily)) {
		print 'div.post-header, h2.post-title, p.post-date-single, h2.post-title-single, div.post-entry h1, div.post-entry h2, div.post-entry h3, div.post-entry h4, div.post-entry h5, div.post-entry h6, div.post-entry blockquote, div.post-footer, h3#comment-count, h4#comment-header, div#comments ol li p.comment-metadata, h4#respond { font-family: ';
		print $barthelme_headerfontfamily;
		print "; }\n";
	}
	else {
		print 'div#content h1, div#content h2 { font-family: ';
		print 'georgia, times, serif';
		print "; }\n";
	}	
}

function barthelme_postentryalignment() {
	if (isset($barthelme_postentryalignment)) {
		print 'div#content p { text-align: ';
		print $barthelme_postentryalignment;
		print "; }\n";
	}
	else {
		print 'div#content p { text-align: ';
		print 'left';
		print "; }\n";
	}
	
}

function barthelme_wrapperwidth() {
	if (isset($barthelme_wrapperwidth)) {
		print 'div#wrapper { width: ';
		print $barthelme_wrapperwidth;
		print "; }\n";
	}
	else {
		print 'div#wrapper { width: ';
		print '90%';
		print "; }\n";
	}
}

function barthelme_sidebartext() {
	if (empty($barthelme_sidebartext) && $barthelme_sidebartext_visible) {
		print '<li><h2>About</h2><p>Lorem ipusm text here can be customized in the <em>Presentation > blog.txt Themes Options</em> menu. You can also select within the options to exclude this section completely. <em>Most</em> XHTML <strong>tags</strong> will <span style="text-decoration:underline;">work</span>.</p></li>';
	}
}

function barthelme_copyrightyear() {
	print date("Y");
}
