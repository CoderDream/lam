/**

$Id$

  This code is part of LDAP Account Manager (http://www.ldap-account-manager.org/)
  Copyright (C) 2003 - 2013  Roland Gruber

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
 * Called when user clicks on a table row. This toggles the checkbox in the row.
 * 
 * @param box checkbox name
 */
function list_click(box) {
	var cbox = document.getElementsByName(box)[0];
	if (cbox.checked == true) {
		cbox.checked = false;
	}
	else {
		cbox.checked = true;
	}
}

/**
 * The user changed the value in the OU selection box. This will reload the list view with the new suffix.
 * 
 * @param type account type
 * @param element dropdown box
 */
function listOUchanged(type, element) {
	location.href='list.php?type=' + type + '&suffix=' + element.options[element.selectedIndex].value;
}

/**
 * Resizes the content area of the account lists to fit the window size.
 * This prevents that the whole page is scrolled in the browser. Only the account table has scroll bars.
 */
function listResizeITabContentDiv() {
	var myDiv = document.getElementById("listTabContentArea");
    var height = document.documentElement.clientHeight;
    height -= myDiv.offsetTop;
    height -= 105;
    myDiv.style.height = height +"px";

    var myDivScroll = document.getElementById("listScrollArea");
	var top = myDivScroll.offsetTop;
	var scrollHeight = height - (top - myDiv.offsetTop);
	myDivScroll.style.height = scrollHeight + "px";
};

/**
 * Shows the dialog to change the list settings.
 * 
 * @param title dialog title
 * @param okText text for Ok button
 * @param cancelText text for Cancel button
 */
function listShowSettingsDialog(title, okText, cancelText) {
	var buttonList = {};
	buttonList[okText] = function() { document.forms["settingsDialogForm"].submit(); };
	buttonList[cancelText] = function() { jQuery(this).dialog("close"); };
	jQuery('#settingsDialog').dialog({
		modal: true,
		title: title,
		dialogClass: 'defaultBackground',
		buttons: buttonList,
		width: 'auto'
	});
}

/**
 * Submits the form by clicking on the given button if enter was pressed.
 * Example: SubmitForm('apply_filter', event);
 * 
 * @param id button ID
 * @param e event
 * @returns Boolean result
 */
function SubmitForm(id, e) {
	if (e.keyCode == 13) {
		if (e.preventDefault) {
			e.preventDefault();
		}
		if (e.returnValue) {
			e.returnValue = false;
		}
		if (window.lastKeyCode) {
			// no submit if last key code was arrow key (browser autocompletion)
			if (window.lastKeyCode == 33 || window.lastKeyCode == 34 ||
				window.lastKeyCode == 38 || window.lastKeyCode == 40) {
				window.lastKeyCode = e.keyCode;
				return true;
			}
		}
		document.getElementsByName(id)[0].click();
		return false;
	}
	window.lastKeyCode = e.keyCode;
	return true;
}

function addResizeHandler(item, min, max) {
	jQuery(item).click(
		function() {
			if (jQuery(item).hasClass('imgExpanded')) {
				jQuery(item).animate({
					height: min
				});
			}
			else {
				jQuery(item).animate({
					height: max
				});
			}
			jQuery(item).toggleClass('imgExpanded');
		}
	);	
}

/**
 * Selects/deselects all accounts on the page.
 */
function list_switchAccountSelection() {
	// set checkbox selection
	jQuery('input.accountBoxUnchecked').prop('checked', true);
	jQuery('input.accountBoxChecked').prop('checked', false);
	// switch CSS class
	nowChecked = jQuery('.accountBoxUnchecked');
	nowUnchecked = jQuery('.accountBoxChecked');
	nowChecked.addClass('accountBoxChecked');
	nowChecked.removeClass('accountBoxUnchecked');
	nowUnchecked.addClass('accountBoxUnchecked');
	nowUnchecked.removeClass('accountBoxChecked');
}

/**
 * The user changed the value in the profile selection box. This will reload the login page with the new profile.
 * 
 * @param element dropdown box
 */
function loginProfileChanged(element) {
	location.href='login.php?useProfile=' + element.options[element.selectedIndex].value;
}

/**
 * Shows the dialog to delete a profile.
 * 
 * @param title dialog title
 * @param okText text for Ok button
 * @param cancelText text for Cancel button
 * @param scope account type (e.g. user)
 * @param selectFieldName name of select box with profile name
 */
function profileShowDeleteDialog(title, okText, cancelText, scope, selectFieldName) {
	// get profile name
	var profileName = jQuery('[name=' + selectFieldName + ']').val();
	// update text
	jQuery('#deleteText').text(profileName);
	// update hidden input fields
	jQuery('#profileDeleteType').val(scope);
	jQuery('#profileDeleteName').val(profileName);
	var buttonList = {};
	buttonList[okText] = function() { document.forms["deleteProfileForm"].submit(); };
	buttonList[cancelText] = function() { jQuery(this).dialog("close"); };
	jQuery('#deleteProfileDialog').dialog({
		modal: true,
		title: title,
		dialogClass: 'defaultBackground',
		buttons: buttonList,
		width: 'auto'
	});
}

/**
 * Shows the dialog to create an automount map.
 * 
 * @param title dialog title
 * @param okText text for Ok button
 * @param cancelText text for Cancel button
 */
function automountShowNewMapDialog(title, okText, cancelText) {
	var buttonList = {};
	buttonList[okText] = function() { document.forms["newAutomountMapDialogForm"].submit(); };
	buttonList[cancelText] = function() { jQuery(this).dialog("close"); };
	jQuery('#newAutomountMapDialog').dialog({
		modal: true,
		title: title,
		dialogClass: 'defaultBackground',
		buttons: buttonList,
		width: 'auto'
	});
}

/**
 * Shows the dialog to change the password.
 * 
 * @param title dialog title
 * @param okText text for Ok button
 * @param cancelText text for Cancel button
 * @param randomText text for random password
 * @param ajaxURL URL used for AJAX request
 */
function passwordShowChangeDialog(title, okText, cancelText, randomText, ajaxURL) {
	var buttonList = {};
	buttonList[okText] = function() { passwordHandleInput("false", ajaxURL); };
	buttonList[randomText] = function() { passwordHandleInput("true", ajaxURL); };
	buttonList[cancelText] = function() {
		jQuery('#passwordDialogMessageArea').html("");
		jQuery(this).dialog("close");
	};
	jQuery('#passwordDialog').dialog({
		modal: true,
		title: title,
		dialogClass: 'defaultBackground',
		buttons: buttonList,
		width: 'auto'
	});
	// set focus on password field
	var myElement = document.getElementsByName('newPassword1')[0];
	myElement.focus();
}

/**
 * Manages the password change when a button is pressed.
 * 
 * @param random "true" if random password should be generated
 * @param ajaxURL URL used for AJAX request
 */
function passwordHandleInput(random, ajaxURL) {
	// get input values
	var modules = new Array();
	jQuery('#passwordDialog').find(':checked').each(function() {
		modules.push(jQuery(this).prop('name'));
	});
	var pwd1 = jQuery('#passwordDialog').find('[name=newPassword1]').val();
	var pwd2 = jQuery('#passwordDialog').find('[name=newPassword2]').val();
	var forcePasswordChange = jQuery('input[name=lamForcePasswordChange]').prop('checked');
	var sendMail = jQuery('input[name=lamPasswordChangeSendMail]').prop('checked');
	var sendMailAlternateAddress = jQuery('#passwordDialog').find('[name=lamPasswordChangeSendMailAddress]').val();
	var pwdJSON = {
		"modules": modules,
		"password1": pwd1,
		"password2": pwd2,
		"random": random,
		"forcePasswordChange": forcePasswordChange,
		"sendMail": sendMail,
		"sendMailAlternateAddress": sendMailAlternateAddress
	};
	// make AJAX call
	jQuery.post(ajaxURL, {jsonInput: pwdJSON}, function(data) {passwordHandleReply(data);}, 'json');
}

/**
 * Manages the server reply to a password change request.
 * 
 * @param data JSON reply
 */
function passwordHandleReply(data) {
	if (data.errorsOccured == "false") {
		jQuery('#passwordDialogMessageArea').html("");
		jQuery('#passwordDialog').dialog("close");
		jQuery('#passwordMessageArea').html(data.messages);
		if (data.forcePasswordChange) {
			jQuery('#forcePasswordChangeOption').attr('checked', 'checked');
		}
	}
	else {
		jQuery('#passwordDialogMessageArea').html(data.messages);
	}	
}

/**
 * Shows a general confirmation dialog and submits a form if the user accepted.
 * 
 * @param title dialog title
 * @param okText text for Ok button
 * @param cancelText text for Cancel button
 * @param dialogDiv div that contains dialog content
 * @param formName form to submit
 * @param resultField (hidden) input field whose value is set to ok/cancel when button is pressed
 */
function showConfirmationDialog(title, okText, cancelText, dialogDiv, formName, resultField) {
	var buttonList = {};
	buttonList[okText] = function() {
		jQuery('#' + dialogDiv).dialog('close');
		if (resultField) {
			jQuery('#' + resultField).val('ok');
		};
		var inputs = jQuery('#' + dialogDiv + ' :input');
		inputs.each(function() {
			jQuery(this).appendTo(document.forms[formName]);
	    });
		document.forms[formName].submit();
	};
	buttonList[cancelText] = function() {
		if (resultField) {
			jQuery('#' + resultField).val('cancel');
		};
		jQuery(this).dialog("close");
	};
	jQuery('#' + dialogDiv).dialog({
		modal: true,
		title: title,
		dialogClass: 'defaultBackground',
		buttons: buttonList,
		width: 'auto'
	});
}

/**
 * Shows a simple confirmation dialog.
 * If the user presses Cancel then the current action is stopped (event.preventDefault()).
 * 
 * @param text dialog text
 * @param e event
 */
function confirmOrStopProcessing(text, e) {
	if (!confirm(text)) {
		if (e.preventDefault) {
			e.preventDefault();
		}
		if (e.returnValue) {
			e.returnValue = false;
		}
		return false;
	}
	return true;
}

/**
 * Alines the elements with the given IDs to the same width.
 * 
 * @param elementIDs IDs
 */
function equalWidth(elementIDs) {
	var maxWidth = 0;
	for (var i = 0; i < elementIDs.length; ++i) {
		if (jQuery(elementIDs[i]).width() > maxWidth) {
			maxWidth = jQuery(elementIDs[i]).width();
		};
	}
	for (var i = 0; i < elementIDs.length; ++i) {
		jQuery(elementIDs[i]).css({'width': maxWidth - (jQuery(elementIDs[i]).outerWidth() - jQuery(elementIDs[i]).width())});
	}
}

/**
 * Alines the elements with the given IDs to the same height.
 * 
 * @param elementIDs IDs
 */
function equalHeight(elementIDs) {
	var max = 0;
	for (var i = 0; i < elementIDs.length; ++i) {
		if (jQuery(elementIDs[i]).height() > max) {
			max = jQuery(elementIDs[i]).height();
		};
	}
	for (var i = 0; i < elementIDs.length; ++i) {
		jQuery(elementIDs[i]).css({'height': max - (jQuery(elementIDs[i]).outerHeight() - jQuery(elementIDs[i]).height())});
	}
}

/**
 * Shows the dialog to change the list settings.
 * 
 * @param title dialog title
 * @param okText text for Ok button
 * @param cancelText text for Cancel button
 * @param scope account type
 * @param selectFieldName name of select box with profile name
 * @param serverProfile profile name
 */
function showDistributionDialog(title, okText, cancelText, scope, type, selectFieldName, serverProfile) {
	// show dialog
	var buttonList = {};
	var dialogId = '';
	
	if (type == 'export') {
		// show structure name to export
		jQuery('#exportName').text(jQuery('[name=' + selectFieldName + ']').val());
		dialogId = 'exportDialog';
		buttonList[okText] = function() { document.forms["exportDialogForm"].submit(); };
		jQuery('<input>').attr({
		    type: 'hidden',
		    name: 'exportProfiles[]',
		    value: serverProfile + '##' + jQuery('[name=' + selectFieldName + ']').val()
		}).appendTo('form');
		jQuery('<input>').attr({
		    type: 'hidden',
		    name: 'scope',
		    value: scope
		}).appendTo('form');
	} else if (type == 'import') {
		dialogId = 'importDialog_' + scope;
		buttonList[okText] = function() { document.forms["importDialogForm_" + scope].submit(); };
	}
	buttonList[cancelText] = function() { jQuery(this).dialog("close"); };
	
	jQuery('#' + dialogId).dialog({
		modal: true,
		title: title,
		dialogClass: 'defaultBackground',
		buttons: buttonList,
		width: 'auto'
	});
	if (type == 'export') {
		equalWidth(new Array('#passwd', '#destServerProfiles'));
	} else if (type == 'import') {
		equalWidth(new Array('#passwd_' + scope, '#importProfiles_' + scope));
	}
}

/**
 * Stores the current scroll position in the form.
 * 
 * @param formName ID of form
 */
function saveScrollPosition(formName) {
	var top = jQuery(window).scrollTop();
	var left = jQuery(window).scrollLeft();
	jQuery('<input>').attr({
	    type: 'hidden',
	    name: 'scrollPositionTop',
	    value: top
	}).appendTo(jQuery('#' + formName));
	jQuery('<input>').attr({
	    type: 'hidden',
	    name: 'scrollPositionLeft',
	    value: left
	}).appendTo(jQuery('#' + formName));
} 

/**
 * Shows the dialog to create a DNS zone.
 * 
 * @param title dialog title
 * @param okText text for Ok button
 * @param cancelText text for Cancel button
 */
function bindShowNewZoneDialog(title, okText, cancelText) {
	var buttonList = {};
	buttonList[okText] = function() { document.forms["newBindZoneDialogForm"].submit(); };
	buttonList[cancelText] = function() { jQuery(this).dialog("close"); };
	jQuery('#newBindZoneDialog').dialog({
		modal: true,
		title: title,
		dialogClass: 'defaultBackground',
		buttons: buttonList,
		width: 'auto'
	});
}



jQuery(document).ready(
	function() {
		jQuery(document).tooltip({
			items: "[helpdata]",
			content: function() {
				var element = $(this);
				var helpString = "<table><tr><th class=\"help\">";
				helpString += element.attr("helptitle");
				helpString += "</th></tr><td class=\"help\">";
				helpString += element.attr("helpdata");
				helpString += "</td></tr></table>";
				return helpString;
			}
		})
	}
);
