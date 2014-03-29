<?php
/*
$Id$

  This code is part of LDAP Account Manager (http://www.ldap-account-manager.org/)
  Copyright (C) 2004 - 2013  Roland Gruber

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/


 /**
* confmodules lets the user select the account modules
*
* @package configuration
* @author Roland Gruber
*/


/** Access to config functions */
include_once('../../lib/config.inc');
/** Access to module lists */
include_once('../../lib/modules.inc');

// start session
if (strtolower(session_module_name()) == 'files') {
	session_save_path("../../sess");
}
@session_start();

setlanguage();


// check if config is set
// if not: load login page
if (!isset($_SESSION['conf_config'])) {
	/** go back to login if password is invalid */
	require('conflogin.php');
	exit;
}

// check if user canceled editing
if (isset($_POST['cancelSettings'])) {
	metaRefresh("../login.php");
	exit;
}

$conf = &$_SESSION['conf_config'];

$errorsToDisplay = checkInput();

// check if button was pressed and if we have to save the settings or go to another tab
if (isset($_POST['saveSettings']) || isset($_POST['editmodules'])
	|| isset($_POST['edittypes']) || isset($_POST['generalSettingsButton'])
	|| isset($_POST['moduleSettings'])) {
	if (sizeof($errorsToDisplay) == 0) {
		// go to final page
		if (isset($_POST['saveSettings'])) {
			metaRefresh("confsave.php");
			exit;
		}
		// go to types page
		elseif (isset($_POST['edittypes'])) {
			metaRefresh("conftypes.php");
			exit;
		}
		// go to general page
		elseif (isset($_POST['generalSettingsButton'])) {
			metaRefresh("confmain.php");
			exit;
		}
		// go to module settings page
		elseif (isset($_POST['moduleSettings'])) {
			metaRefresh("moduleSettings.php");
			exit;
		}
	}
}

$types = $conf->get_ActiveTypes();

echo $_SESSION['header'];

echo "<title>" . _("LDAP Account Manager Configuration") . "</title>\n";

// include all CSS files
$cssDirName = dirname(__FILE__) . '/../../style';
$cssDir = dir($cssDirName);
$cssFiles = array();
$cssEntry = $cssDir->read();
while ($cssEntry !== false) {
	if (substr($cssEntry, strlen($cssEntry) - 4, 4) == '.css') {
		$cssFiles[] = $cssEntry;
	}
	$cssEntry = $cssDir->read();
}
sort($cssFiles);
foreach ($cssFiles as $cssEntry) {
	echo "<link rel=\"stylesheet\" type=\"text/css\" href=\"../../style/" . $cssEntry . "\">\n";
}

echo "<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"../../graphics/favicon.ico\">\n";
echo "</head><body>\n";
// include all JavaScript files
$jsDirName = dirname(__FILE__) . '/../lib';
$jsDir = dir($jsDirName);
$jsFiles = array();
while ($jsEntry = $jsDir->read()) {
	if (substr($jsEntry, strlen($jsEntry) - 3, 3) != '.js') continue;
	$jsFiles[] = $jsEntry;
}
sort($jsFiles);
foreach ($jsFiles as $jsEntry) {
	echo "<script type=\"text/javascript\" src=\"../lib/" . $jsEntry . "\"></script>\n";
}

?>
		<table border=0 width="100%" class="lamHeader ui-corner-all">
			<tr>
				<td align="left" height="30">
					<a class="lamLogo" href="http://www.ldap-account-manager.org/" target="new_window">LDAP Account Manager</a>
				</td>
				<td align="right">
					<?php echo _('Server profile') . ': ' . $conf->getName(); ?>
					&nbsp;&nbsp;
				</td>
			</tr>
		</table>
		<br>
<?php

// print error messages
for ($i = 0; $i < sizeof($errorsToDisplay); $i++) call_user_func_array('StatusMessage', $errorsToDisplay[$i]);

echo ("<form id=\"inputForm\" action=\"confmodules.php\" method=\"post\" onSubmit=\"saveScrollPosition('inputForm')\">\n");

// hidden submit buttons which are clicked by tabs
echo "<div style=\"display: none;\">\n";
	echo "<input name=\"generalSettingsButton\" type=\"submit\" value=\" \">";
	echo "<input name=\"edittypes\" type=\"submit\" value=\" \">";
	echo "<input name=\"editmodules\" type=\"submit\" value=\" \">";
	echo "<input name=\"moduleSettings\" type=\"submit\" value=\" \">";
echo "</div>\n";
	
// tabs
echo '<div class="ui-tabs ui-widget ui-widget-content ui-corner-all">';

echo '<ul class="ui-tabs-nav ui-helper-reset ui-helper-clearfix ui-widget-header ui-corner-all">';
echo '<li id="generalSettingsButton" class="ui-state-default ui-corner-top" onmouseover="jQuery(this).addClass(\'tabs-hover\');" onmouseout="jQuery(this).removeClass(\'tabs-hover\');">';
	echo '<a href="#" onclick="document.getElementsByName(\'generalSettingsButton\')[0].click();"><img src="../../graphics/tools.png" alt=""> ';
	echo _('General settings') . '</a>';
echo '</li>';
echo '<li id="edittypes" class="ui-state-default ui-corner-top" onmouseover="jQuery(this).addClass(\'tabs-hover\');" onmouseout="jQuery(this).removeClass(\'tabs-hover\');">';
	echo '<a href="#" onclick="document.getElementsByName(\'edittypes\')[0].click();"><img src="../../graphics/gear.png" alt=""> ';
	echo _('Account types') . '</a>';
echo '</li>';
echo '<li id="editmodules" class="ui-state-default ui-corner-top">';
	echo '<a href="#" onclick="document.getElementsByName(\'editmodules\')[0].click();"><img src="../../graphics/modules.png" alt=""> ';
	echo _('Modules') . '</a>';
echo '</li>';
echo '<li id="moduleSettings" class="ui-state-default ui-corner-top" onmouseover="jQuery(this).addClass(\'tabs-hover\');" onmouseout="jQuery(this).removeClass(\'tabs-hover\');">';
	echo '<a href="#" onclick="document.getElementsByName(\'moduleSettings\')[0].click();"><img src="../../graphics/modules.png" alt=""> ';
	echo _('Module settings') . '</a>';
echo '</li>';
echo '</ul>';

?>

<script type="text/javascript">
jQuery(document).ready(function() {
	jQuery('#editmodules').addClass('ui-tabs-active');
	jQuery('#editmodules').addClass('ui-state-active');
	jQuery('#editmodules').addClass('user-bright');
	// set common width for select boxes
	var maxWidth = 0;
	jQuery("select").each(function(){
		if (jQuery(this).width() > maxWidth)
		maxWidth = jQuery(this).width();   
	});
	jQuery("select").width(maxWidth);
});
</script>

<div class="ui-tabs-panel ui-widget-content ui-corner-bottom user-bright">
<?php


$account_list = array();
for ($i = 0; $i < sizeof($types); $i++) {
	$account_list[] = array($types[$i], getTypeAlias($types[$i]));
}

$container = new htmlTable();
for ($i = 0; $i < sizeof($account_list); $i++) {
	config_showAccountModules($account_list[$i][0], $account_list[$i][1], $container);
}

$legendContainer = new htmlTable();
$legendContainer->addElement(new htmlOutputText("(*) " . _("Base module")));
$legendContainer->addElement(new htmlHelpLink('237'));
$container->addElement($legendContainer);
$container->addElement(new htmlHiddenInput('postAvailable', 'yes'));

$tabindex = 1;
parseHtml(null, $container, array(), false, $tabindex, 'user');

echo "</div></div>";

$buttonContainer = new htmlTable();
$buttonContainer->addElement(new htmlSpacer(null, '10px'), true);
$saveButton = new htmlButton('saveSettings', _('Save'));
$saveButton->setIconClass('saveButton');
$buttonContainer->addElement($saveButton);
$cancelButton = new htmlButton('cancelSettings', _('Cancel'));
$cancelButton->setIconClass('cancelButton');
$buttonContainer->addElement($cancelButton, true);
$buttonContainer->addElement(new htmlSpacer(null, '10px'), true);
parseHtml(null, $buttonContainer, array(), false, $tabindex, 'user');

if ((sizeof($errorsToDisplay) == 0) && isset($_POST['scrollPositionTop']) && isset($_POST['scrollPositionLeft'])) {
	// scroll to last position
	echo '<script type="text/javascript">
		jQuery(document).ready(function() {
			jQuery(window).scrollTop(' . $_POST['scrollPositionTop'] . ');
			jQuery(window).scrollLeft('. $_POST['scrollPositionLeft'] . ');
	});
	</script>';
}

echo "</form>\n";
echo "</body>\n";
echo "</html>\n";


/**
* Displays the module selection boxes and checks if dependencies are fulfilled.
*
* @param string $scope account type
* @param string $title title for module selection (e.g. "User modules")
* @param htmlTable $container meta HTML container
*/
function config_showAccountModules($scope, $title, &$container) {
	$conf = &$_SESSION['conf_config'];
	$typeSettings = $conf->get_typeSettings();
	// account modules
	$available = getAvailableModules($scope, true);
	$selected = $typeSettings['modules_' . $scope];
	if (isset($selected) && ($selected != '')) {
		$selected = explode(',', $selected);
	}
	else {
		$selected = array();
	}
	$sortedAvailable = array();
	for ($i = 0; $i < sizeof($available); $i++) {
		$sortedAvailable[$available[$i]] = getModuleAlias($available[$i], $scope);
	}
	natcasesort($sortedAvailable);

	// build options for selected and available modules
	$selOptions = array();
	for ($i = 0; $i < sizeof($selected); $i++) {
		if (in_array($selected[$i], $available)) {  // selected modules must be available
			if (is_base_module($selected[$i], $scope)) {  // mark base modules
				$selOptions[getModuleAlias($selected[$i], $scope) . " (" . $selected[$i] .  ")(*)"] = $selected[$i];
			}
			else {
				$selOptions[getModuleAlias($selected[$i], $scope) . " (" . $selected[$i] .  ")"] = $selected[$i];
			}
		}
	}
	$availOptions = array();
	foreach ($sortedAvailable as $key => $value) {
		if (! in_array($key, $selected)) {  // display non-selected modules
			if (is_base_module($key, $scope)) {  // mark base modules
				$availOptions[$value . " (" . $key .  ")(*)"] = $key;
			}
			else {
				$availOptions[$value . " (" . $key .  ")"] = $key;
			}
		}
	}
	
	// add account module selection
	$container->addElement(new htmlSubTitle($title, '../../graphics/' . $scope . '.png'), true);
	$container->addElement(new htmlOutputText(_("Selected modules")));
	// add/remove buttons
	$buttonContainer = new htmlTable();
	$buttonContainer->rowspan = 2;
	if (sizeof($availOptions) > 0) {
		$addButton = new htmlButton($scope . "_add", 'back.gif', true);
		$addButton->setTitle(_('Add'));
		$buttonContainer->addElement($addButton, true);
	}
	if (sizeof($selOptions) > 0) {
		$remButton = new htmlButton($scope . "_remove", 'forward.gif', true);
		$remButton->setTitle(_('Remove'));
		$buttonContainer->addElement($remButton, true);
	}
	$container->addElement($buttonContainer);
	$container->addElement(new htmlOutputText(_("Available modules")), true);
	// selected modules
	if (sizeof($selOptions) > 0) {
		$selSelect = new htmlSelect($scope . '_selected', $selOptions, array(), 5);
		$selSelect->setTransformSingleSelect(false);
		$selSelect->setMultiSelect(true);
		$selSelect->setHasDescriptiveElements(true);
		$selSelect->setSortElements(false);
		$container->addElement($selSelect);
	}
	else {
		$container->addElement(new htmlOutputText(''));
	}
	// available modules
	if (sizeof($availOptions) > 0) {
		$availSelect = new htmlSelect($scope . "_available", $availOptions, array(), 5);
		$availSelect->setTransformSingleSelect(false);
		$availSelect->setHasDescriptiveElements(true);
		$availSelect->setMultiSelect(true);
		$container->addElement($availSelect, true);
	}
	// spacer to next account type
	$container->addElement(new htmlSpacer(null, '30px'), true);
}

/**
 * Checks user input and saves the entered settings.
 *
 * @return array list of errors
 */
function checkInput() {
	if (!isset($_POST['postAvailable'])) {
		return array();
	}
	$errors = array();
	$conf = &$_SESSION['conf_config'];
	$typeSettings = $conf->get_typeSettings();
	$accountTypes = $conf->get_ActiveTypes();
	for ($t = 0; $t < sizeof($accountTypes); $t++) {
		$scope = $accountTypes[$t];
		$available = getAvailableModules($scope, true);
		$selected_temp = $typeSettings['modules_' . $scope];
		if (isset($selected_temp)) $selected_temp = explode(',', $selected_temp);
		$selected = array();
		// only use available modules as selected
		for ($i = 0; $i < sizeof($selected_temp); $i++) {
			if (in_array($selected_temp[$i], $available)) {
				$selected[] = $selected_temp[$i];
			}
		}
		// remove modules from selection
		if (isset($_POST[$scope . '_selected']) && isset($_POST[$scope . '_remove'])) {
			$new_selected = array();
			for ($i = 0; $i < sizeof($selected); $i++) {
				if (! in_array($selected[$i], $_POST[$scope . '_selected'])) $new_selected[] = $selected[$i];
			}
			$selected = $new_selected;
			$typeSettings['modules_' . $scope] = implode(',', $selected);
		}
		// add modules to selection
		elseif (isset($_POST[$scope . '_available']) && isset($_POST[$scope . '_add'])) {
			$new_selected = $selected;
			for ($i = 0; $i < sizeof($_POST[$scope . '_available']); $i++) {
				if (! in_array($_POST[$scope . '_available'][$i], $selected)) $new_selected[] = $_POST[$scope . '_available'][$i];
			}
			$selected = $new_selected;
			$typeSettings['modules_' . $scope] = implode(',', $selected);
		}
		// check dependencies
		$depends = check_module_depends($selected, getModulesDependencies($scope));
		if ($depends != false) {
			for ($i = 0; $i < sizeof($depends); $i++) {
				$errors[] = array('ERROR', getTypeAlias($scope), _("Unsolved dependency:") . ' ' .
					$depends[$i][0] . " (" . $depends[$i][1] . ")");
			}
		}
		// check conflicts
		$conflicts = check_module_conflicts($selected, getModulesDependencies($scope));
		if ($conflicts != false) {
			for ($i = 0; $i < sizeof($conflicts); $i++) {
				$errors[] = array('ERROR', getTypeAlias($scope), _("Conflicting module:") . ' ' .
					$conflicts[$i][0] . " (" . $conflicts[$i][1] . ")");
			}
		}
		// check for base module
		$baseCount = 0;
		for ($i = 0; $i < sizeof($selected); $i++) {
			if (is_base_module($selected[$i], $scope)) {
				$baseCount++;
			}
		}
		if ($baseCount != 1) {
			$errors[] = array('ERROR', getTypeAlias($scope), _("No or more than one base module selected!"));
		}	
	}
	$conf->set_typeSettings($typeSettings);

	return $errors;
}

?>




