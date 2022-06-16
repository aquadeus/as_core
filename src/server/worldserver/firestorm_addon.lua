--[FUNCTION]

SetVariables = function()
	g_Referer = "";
	g_RewardCount = 0;
	g_Recruits = {};
	g_RecruitIndex = 1;
	g_RecruitOffset = 1;
	g_RecruitFrames = {};
	g_BenefitCount = 0;
	g_LastBenefit = nil;
	g_NotificationsEnabled = true

	g_ChangelogItemCount = 0;
	g_ChangelogItems = {};

	g_NewsFeedItemCount = 0;
	g_NewsFeedItems = {};
	
	g_AlertButtonText= nil;

	g_Language = nil;
	g_CurrentLanguageButton = 0;

	FS_RAF_PRODUCTS = {}

	StaticPopupDialogs["CONFIRM_ACTIVATE_RAF"] = {
	text = CONFIRM_CONTINUE,
	button1 = OKAY,
	button2 = CANCEL,
	OnAccept = function(self, data) SelectCharacter(); end,
	hideOnEscape = 1,
	timeout = 0,

	ResetFrameVariables()
	}
	
end

--[FUNCTION]

ResetFrameVariables = function()
	g_Frame = nil
	g_Layer = nil
	g_SubFrame = nil
	g_SubSubFrame = nil
	g_ScrollChild = nil
	g_ScrollFrame = nil
end

--[FUNCTION]

SetBorders = function(p_Parent, p_ParentName)
	g_Layer = p_Parent:CreateTexture(p_ParentName .. "TopLeft", "BACKGROUND")
	g_Layer:SetTexture("Interface\\Common\\Common-Input-Border-TL")
	g_Layer:SetSize(8, 8)
	g_Layer:SetPoint("TOPLEFT", 0, 0)

	g_Layer = p_Parent:CreateTexture(p_ParentName .. "TopRight", "BACKGROUND")
	g_Layer:SetTexture("Interface\\Common\\Common-Input-Border-TR")
	g_Layer:SetSize(8, 8)
	g_Layer:SetPoint("TOPRIGHT", 0, 0)

	g_Layer = p_Parent:CreateTexture(p_ParentName .. "Top", "BACKGROUND")
	g_Layer:SetTexture("Interface\\Common\\Common-Input-Border-T")
	g_Layer:SetPoint("TOPLEFT",_G[p_ParentName .. "TopLeft"], "TOPRIGHT", 0, 0)
	g_Layer:SetPoint("BOTTOMRIGHT",_G[p_ParentName .. "TopRight"], "BOTTOMLEFT", 0, 0)

	g_Layer = p_Parent:CreateTexture(p_ParentName .. "BottomLeft", "BACKGROUND")
	g_Layer:SetTexture("Interface\\Common\\Common-Input-Border-BL")
	g_Layer:SetSize(8, 8)
	g_Layer:SetPoint("BOTTOMLEFT", 0, 0)

	g_Layer = p_Parent:CreateTexture(p_ParentName .. "BottomRight", "BACKGROUND")
	g_Layer:SetTexture("Interface\\Common\\Common-Input-Border-BR")
	g_Layer:SetSize(8, 8)
	g_Layer:SetPoint("BOTTOMRIGHT", 0, 0)

	g_Layer = p_Parent:CreateTexture(p_ParentName .. "Bottom", "BACKGROUND")
	g_Layer:SetTexture("Interface\\Common\\Common-Input-Border-B")
	g_Layer:SetPoint("TOPLEFT",_G[p_ParentName .. "BottomLeft"], "TOPRIGHT", 0, 0)
	g_Layer:SetPoint("BOTTOMRIGHT",_G[p_ParentName .. "BottomRight"], "BOTTOMLEFT", 0, 0)
	
	g_Layer = p_Parent:CreateTexture(p_ParentName .. "Left", "BACKGROUND")
	g_Layer:SetTexture("Interface\\Common\\Common-Input-Border-L")
	g_Layer:SetPoint("TOPLEFT",_G[p_ParentName .. "TopLeft"], "BOTTOMLEFT", 0, 0)
	g_Layer:SetPoint("BOTTOMRIGHT",_G[p_ParentName .. "BottomLeft"], "TOPRIGHT", 0, 0)

	g_Layer = p_Parent:CreateTexture(p_ParentName .. "Right", "BACKGROUND")
	g_Layer:SetTexture("Interface\\Common\\Common-Input-Border-R")
	g_Layer:SetPoint("TOPLEFT",_G[p_ParentName .. "TopRight"], "TOPRIGHT", 0, 0)
	g_Layer:SetPoint("BOTTOMRIGHT",_G[p_ParentName .. "BottomRight"], "BOTTOMLEFT", 0, 0)

	g_Layer = p_Parent:CreateTexture(p_ParentName .. "Middle", "BACKGROUND")
	g_Layer:SetTexture("Interface\\Common\\Common-Input-Border-M")
	g_Layer:SetPoint("TOPLEFT",_G[p_ParentName .. "Left"], "TOPRIGHT", 0, 0)
	g_Layer:SetPoint("BOTTOMRIGHT",_G[p_ParentName .. "Right"], "BOTTOMLEFT", 0, 0)
end

--[FUNCTION]

CreateMainRAFFrame_1 = function()
	g_Frame = CreateFSRAFNotificationFrame("FSRecruitAFriendFrame", "CENTER", 0, 0)
	g_Frame:SetSize(485, 265)
	g_Frame:Hide();

	g_Layer = g_Frame:CreateFontString(nil, 'ARTWORK', "GameFontNormal")
	g_Layer:SetText(RECRUIT_A_FRIEND)
	g_Layer:SetPoint("TOP", 0, -5)

	g_Layer = g_Frame:CreateFontString(nil, 'ARTWORK', "GameFontNormal")
	g_Layer:SetJustifyV("MIDDLE")
	g_Layer:SetText(RAF_DESCRIPTION)
	g_Layer:SetSize(400, 40)
	g_Layer:SetPoint("TOP", 0, -25)
	g_Layer:SetTextColor(0.8, 0.8, 0.8)

	g_Layer = g_Frame:CreateFontString("EmailLabel", 'ARTWORK', "GameFontNormal")
	g_Layer:SetText(RAF_ENTER_EMAIL)
	g_Layer:SetPoint("TOPLEFT", 24, -74)

	g_Layer = g_Frame:CreateFontString("NoteLabel", 'ARTWORK', "GameFontNormal")
	g_Layer:SetText(RAF_ENTER_NOTE)
	g_Layer:SetPoint("TOPLEFT", 24, -131)

	g_Layer = g_Frame:CreateFontString("BenefitsLabel", 'ARTWORK', "GameFontNormalLarge")
	g_Layer:SetText(RAF_BENEFITS)
	g_Layer:SetPoint("TOPLEFT", 277, -82)

	g_Layer = g_Frame:CreateFontString("Benefit2", 'ARTWORK', "GameFontHighlight")
	g_Layer:SetJustifyH("LEFT")
	g_Layer:SetText(FS_BENEFIT_PREVIEW_EXPERIENCE)
	g_Layer:SetSize(176, 0)
	g_Layer:SetPoint("TOPLEFT", _G["BenefitsLabel"], "BOTTOMLEFT", 0, -4)
	g_Layer:SetTextColor(0.8, 0.8, 0.8)

	g_Layer = g_Frame:CreateTexture(nil, "ARTWORK")
	g_Layer:SetTexture("Interface\\Scenarios\\ScenarioIcon-Combat")
	g_Layer:SetSize(16, 16)
	g_Layer:SetPoint("TOPRIGHT", _G["Benefit2"], "TOPLEFT", 0, 0)
end

--[FUNCTION]

CreateMainRAFFrame_2 = function()
	g_Layer = g_Frame:CreateFontString("Benefit3", 'ARTWORK', "GameFontHighlight")
	g_Layer:SetJustifyH("LEFT")
	g_Layer:SetText(FS_BENEFIT_PREVIEW_AP)
	g_Layer:SetSize(176, 0)
	g_Layer:SetPoint("TOPLEFT", _G["Benefit2"], "BOTTOMLEFT", 0, -4)
	g_Layer:SetTextColor(0.8, 0.8, 0.8)

	g_Layer = g_Frame:CreateTexture(nil, "ARTWORK")
	g_Layer:SetTexture("Interface\\Scenarios\\ScenarioIcon-Combat")
	g_Layer:SetSize(16, 16)
	g_Layer:SetPoint("TOPRIGHT", _G["Benefit3"], "TOPLEFT", 0, 0)

	g_Layer = g_Frame:CreateFontString("Benefit4", 'ARTWORK', "GameFontHighlight")
	g_Layer:SetJustifyH("LEFT")
	g_Layer:SetText(FS_BENEFIT_PREVIEW_NETHERSHARDS)
	g_Layer:SetSize(176, 0)
	g_Layer:SetPoint("TOPLEFT", _G["Benefit3"], "BOTTOMLEFT", 0, -4)
	g_Layer:SetTextColor(0.8, 0.8, 0.8)

	g_Layer = g_Frame:CreateTexture(nil, "ARTWORK")
	g_Layer:SetTexture("Interface\\Scenarios\\ScenarioIcon-Combat")
	g_Layer:SetSize(16, 16)
	g_Layer:SetPoint("TOPRIGHT", _G["Benefit4"], "TOPLEFT", 0, 0)

	g_Layer = g_Frame:CreateFontString("Benefit5", 'ARTWORK', "GameFontHighlight")
	g_Layer:SetJustifyH("LEFT")
	g_Layer:SetText(FS_BENEFIT_PREVIEW_LOOT)
	g_Layer:SetSize(176, 0)
	g_Layer:SetPoint("TOPLEFT", _G["Benefit4"], "BOTTOMLEFT", 0, -4)
	g_Layer:SetTextColor(0.8, 0.8, 0.8)

	g_Layer = g_Frame:CreateTexture(nil, "ARTWORK")
	g_Layer:SetTexture("Interface\\Scenarios\\ScenarioIcon-Combat")
	g_Layer:SetSize(16, 16)
	g_Layer:SetPoint("TOPRIGHT", _G["Benefit5"], "TOPLEFT", 0, 0)

	g_Layer = g_Frame:CreateFontString("Benefit6", 'ARTWORK', "GameFontHighlight")
	g_Layer:SetJustifyH("LEFT")
	g_Layer:SetText(FS_BENEFIT_PREVIEW_HONOR)
	g_Layer:SetSize(176, 0)
	g_Layer:SetPoint("TOPLEFT", _G["Benefit5"], "BOTTOMLEFT", 0, -4)
	g_Layer:SetTextColor(0.8, 0.8, 0.8)

	g_Layer = g_Frame:CreateTexture(nil, "ARTWORK")
	g_Layer:SetTexture("Interface\\Scenarios\\ScenarioIcon-Combat")
	g_Layer:SetSize(16, 16)
	g_Layer:SetPoint("TOPRIGHT", _G["Benefit6"], "TOPLEFT", 0, 0)

	g_Layer = g_Frame:CreateFontString("Benefit7", 'ARTWORK', "GameFontHighlight")
	g_Layer:SetJustifyH("LEFT")
	g_Layer:SetText(FS_BENEFIT_PREVIEW_LEGENDARY)
	g_Layer:SetSize(176, 0)
	g_Layer:SetPoint("TOPLEFT", _G["Benefit6"], "BOTTOMLEFT", 0, -4)
	g_Layer:SetTextColor(0.8, 0.8, 0.8)

	g_Layer = g_Frame:CreateTexture(nil, "ARTWORK")
	g_Layer:SetTexture("Interface\\Scenarios\\ScenarioIcon-Combat")
	g_Layer:SetSize(16, 16)
	g_Layer:SetPoint("TOPRIGHT", _G["Benefit7"], "TOPLEFT", 0, 0)
end

--[FUNCTION]

CreateMainRAFFrame_3 = function()
	g_SubFrame = CreateFrame("Button", "MoreDetails", g_Frame, "UIPanelButtonTemplate")
	g_SubFrame:SetText(FS_RAF_MORE_DETAILS)
	g_SubFrame:SetSize(176, 30)
	g_SubFrame:SetPoint("TOPLEFT", _G["Benefit6"], "BOTTOMLEFT", -12, -37)
	g_SubFrame:SetScript("OnClick", function(self) MoreDetails_OnClick(self) end)

	g_SubFrame = CreateFrame("EditBox", "FSRecruitAFriendNameEditBox", g_Frame)
	g_SubFrame:SetAutoFocus(false)
	g_SubFrame:SetMaxLetters(200)
	g_SubFrame:SetSize(210, 20)
	g_SubFrame:SetPoint("TOPLEFT", _G["EmailLabel"], "BOTTOMLEFT", 5, -6)

	g_Layer = g_SubFrame:CreateTexture("FSRecruitAFriendNameEditBoxLeft", "BACKGROUND")
	g_Layer:SetTexture("Interface\\Common\\Common-Input-Border")
	g_Layer:SetSize(8, 20)
	g_Layer:SetPoint("TOPLEFT", -5, 0)
	g_Layer:SetTexCoord(0, 0.0625, 0, 0.625)

	g_Layer = g_SubFrame:CreateTexture("FSRecruitAFriendNameEditBoxRight", "BACKGROUND")
	g_Layer:SetTexture("Interface\\Common\\Common-Input-Border")
	g_Layer:SetSize(8, 20)
	g_Layer:SetPoint("RIGHT", 0, 0)
	g_Layer:SetTexCoord(0.9375, 1.0, 0, 0.625)

	g_Layer = g_SubFrame:CreateTexture("FSRecruitAFriendNameEditBoxMiddle", "BACKGROUND")
	g_Layer:SetTexture("Interface\\Common\\Common-Input-Border")
	g_Layer:SetSize(0, 20)
	g_Layer:SetPoint("LEFT", _G["FSRecruitAFriendNameEditBoxLeft"], "RIGHT", 0, 0)
	g_Layer:SetPoint("RIGHT", _G["FSRecruitAFriendNameEditBoxRight"], "LEFT", 0, 0)
	g_Layer:SetTexCoord(0.0625, 0.9375, 0, 0.625)

	g_Layer = g_SubFrame:CreateFontString("FSRecruitAFriendNameEditBoxFill", "BORDER", "FriendsFont_Small")
	g_Layer:SetJustifyH("LEFT")
	g_Layer:SetJustifyV("MIDDLE")
	g_Layer:SetText("ENTER_EMAIL")
	g_Layer:SetPoint("LEFT", 3, 0)
	g_Layer:SetTextColor(0.35, 0.35, 0.35)

	g_SubFrame:SetScript("OnEnterPressed", function(self) FSRecruitAFriendNoteEditBox:SetFocus() end)
	g_SubFrame:SetScript("OnTabPressed", function(self) FSRecruitAFriendNoteEditBox:SetFocus() end)
	g_SubFrame:SetScript("OnEscapePressed", function(self) self:ClearFocus() end)
	g_SubFrame:SetScript("OnTextChanged", function(self) FSRecruitAFriend_OnEmailTextChanged(self, userInput) end)

	g_SubFrame:SetFont("Fonts\\FRIZQT__.TTF", 10)

	g_SubFrame = CreateFrame("Frame", "FSRecruitAFriendNoteFrame", g_Frame)
	g_SubFrame:SetSize(215, 60)
	g_SubFrame:SetPoint("TOPLEFT", _G["NoteLabel"], "BOTTOMLEFT", 0, -5)

	SetBorders(g_SubFrame, "NoteFrame")
end

--[FUNCTION]

CreateMainRAFFrame_4 = function()
	g_ScrollFrame = CreateFrame("ScrollFrame", "FSRecruitAFriendNoteFrameScrollFrame", g_SubFrame, "UIPanelScrollFrameTemplate")
	g_ScrollFrame:SetPoint("TOPLEFT", _G["FSRecruitAFriendNoteFrame"], "TOPLEFT", 6, -6)
	g_ScrollFrame:SetPoint("BOTTOMRIGHT", _G["FSRecruitAFriendNoteFrame"], "BOTTOMRIGHT", 0, 6)
	g_ScrollFrame:SetScript("OnMouseDown", function(self) FSRecruitAFriendNoteEditBox:SetFocus() end)

	g_ScrollChild = CreateFrame("EditBox", "FSRecruitAFriendNoteEditBox", g_ScrollFrame)
	g_ScrollChild:SetAutoFocus(false)
	g_ScrollChild:SetMaxLetters(127)
	g_ScrollChild:SetMultiLine(true)
	g_ScrollChild:SetSize(188, 1)

	g_Layer = g_ScrollChild:CreateFontString("FSRecruitAFriendNoteEditBoxFill", "BORDER", "GameFontNormalSmall")
	g_Layer:SetJustifyH("LEFT")
	g_Layer:SetJustifyV("MIDDLE")
	g_Layer:SetText(RAF_ENTER_NOTE_PROMPT)
	g_Layer:SetPoint("LEFT", 3, 0)
	g_Layer:SetTextColor(0.35, 0.35, 0.35)

	g_ScrollChild:SetScript("OnTabPressed", function(self) FSRecruitAFriendNoteEditBox:SetFocus() end)
	g_ScrollChild:SetScript("OnTextChanged", function(self) FSRecruitAFriend_OnNoteChanged(self) end)
	g_ScrollChild:SetScript("OnUpdate", function(self) ScrollingEdit_OnUpdate(self, elapsed, self:GetParent()) end)
	g_ScrollChild:SetScript("OnEscapePressed", function(self) self:ClearFocus() end)

	g_ScrollChild:SetFont("Fonts\\FRIZQT__.TTF", 10)

	g_ScrollFrame:SetScrollChild(g_ScrollChild)

end

--[FUNCTION]

CreateMainRAFFrame_5 = function()
	g_SubFrame = CreateFrame("Button", "FSRecruitAFriendFrameSendButton", g_Frame, "UIPanelButtonTemplate")
	g_SubFrame:SetText(RAF_SEND_INVITATION)
	g_SubFrame:SetSize(219, 22)
	g_SubFrame:SetPoint("TOPLEFT", _G["FSRecruitAFriendNoteFrame"], "BOTTOMLEFT", -2, -15)
	g_SubFrame:SetScript("OnClick", function(self) FSRecruitAFriend_Send() end)

	g_SubFrame = CreateFrame("Frame", "FSRecruitAFriendBenefitsFrame", g_Frame)
	g_SubFrame:Hide()
	g_SubFrame:SetSize(471, 358)
	g_SubFrame:SetPoint("TOPLEFT", g_Frame, "TOPLEFT", 7, -260)

	SetBorders(g_SubFrame, "FSRecruitAFriendBenefitsFrame")

	g_ScrollFrame = CreateFrame("ScrollFrame", "FSRecruitAFriendBenefitsFrameScrollFrame", g_SubFrame, "UIPanelScrollFrameTemplate")
	g_ScrollFrame:SetPoint("TOPLEFT", _G["FSRecruitAFriendBenefitsFrame"], "TOPLEFT", 6, -6)
	g_ScrollFrame:SetPoint("BOTTOMRIGHT", _G["FSRecruitAFriendBenefitsFrame"], "BOTTOMRIGHT", 0, 6)

	g_ScrollChild = CreateFrame("Frame", "FSRecruitAFriendDetails", g_ScrollFrame)
	g_ScrollChild:SetSize(471, 420)
	g_ScrollFrame:SetScrollChild(g_ScrollChild)

	g_Frame:SetScript("OnShow", function(self) FSRecruitAFriend_OnShow(self) end)
	g_Frame:SetScript("OnHide", function(self) FSRecruitAFriend_OnHide(self) end)
end

--[FUNCTION]

FSScrollFrame_OnLoad = function(self)
	local scrollBar = _G[self:GetName().."ScrollBar"]
	scrollBar:ClearAllPoints()
	scrollBar:SetPoint("TOPLEFT", self, "TOPRIGHT", -18, -10)
	scrollBar:SetPoint("BOTTOMLEFT", self, "BOTTOMRIGHT", -18, 8)
	_G[self:GetName().."ScrollBarScrollDownButton"]:SetPoint("TOP", scrollBar, "BOTTOM", 0, 4)
	_G[self:GetName().."ScrollBarScrollUpButton"]:SetPoint("BOTTOM", scrollBar, "TOP", 0, -4)
	self.scrollBarHideable = 1
	scrollBar:Hide()
end

--[FUNCTION]

CreateMainRAFFrame = function()
	CreateMainRAFFrame_1()
	CreateMainRAFFrame_2()
	CreateMainRAFFrame_3()
	CreateMainRAFFrame_4()
	CreateMainRAFFrame_5()
	AddRAFBenefits()

	FSRecruitAFriend_OnLoad(_G["FSRecruitAFriendFrame"])
	FSScrollFrame_OnLoad(_G["FSRecruitAFriendBenefitsFrameScrollFrame"])
	FSScrollFrame_OnLoad(_G["FSRecruitAFriendNoteFrameScrollFrame"])

	ResetFrameVariables()
end

--[FUNCTION]

AddRAFBenefits = function()
	AddRAFBenefit(FS_BENEFIT_TITLE_EXPERIENCE, FS_BENEFIT_DESC_EXPERIENCE)
	AddRAFBenefit(FS_BENEFIT_TITLE_AP, FS_BENEFIT_DESC_AP)
	AddRAFBenefit(FS_BENEFIT_TITLE_NETHERSHARDS, FS_BENEFIT_DESC_NETHERSHARDS)
	AddRAFBenefit(FS_BENEFIT_TITLE_LOOT, FS_BENEFIT_DESC_LOOT)
	AddRAFBenefit(FS_BENEFIT_TITLE_HONOR, FS_BENEFIT_DESC_HONOR)
	AddRAFBenefit(FS_BENEFIT_TITLE_LEGENDARY, FS_BENEFIT_DESC_LEGENDARY)
	AddRAFBenefit(FS_BENEFIT_TITLE_TELEPORT, FS_BENEFIT_DESC_TELEPORT)
	AddRAFBenefit(FS_BENEFIT_TITLE_REWARDS, FS_BENEFIT_DESC_REWARDS)
end

--[FUNCTION]

CreateRecruitAFriendSentFrame = function()
	g_Frame = CreateFSRAFNotificationFrame("FSRecruitAFriendSentFrame", "CENTER", 0, 0)
	g_Frame:SetSize(385, 156)
	g_Frame:Hide();
	g_Frame:SetScript("OnShow", function(self) _G["FSRAFReward"]:Hide() _G["FSRecruitAFriendFrame"]:Hide() end)

	g_Layer = g_Frame:CreateFontString(nil, 'ARTWORK', "GameFontNormal")
	g_Layer:SetText(RECRUIT_A_FRIEND)
	g_Layer:SetPoint("TOP", 0, -5)

	g_Layer = g_Frame:CreateFontString("FSRecruitAFriendSentFrameDescription", "ARTWORK", "GameFontHighlight")
	g_Layer:SetText(RAF_INVITATION_SENT)
	g_Layer:SetJustifyH("CENTER")
	g_Layer:SetJustifyV("MIDDLE")
	g_Layer:SetSize(300, 200)
	g_Layer:SetPoint("CENTER", _G["FSRecruitAFriendSentFrame"], "CENTER", 0, 15)

	g_SubFrame = CreateFrame("Button", "FSRecruitAFriendSentFrameOKButton", g_Frame, "UIPanelButtonTemplate")
	g_SubFrame:SetText(OKAY)
	g_SubFrame:SetSize(170,22)
	g_SubFrame:SetPoint("BOTTOM", 0, 15)
	g_SubFrame:SetScript("OnClick", function(self) StaticPopupSpecial_Hide(FSRecruitAFriendSentFrame) end)

	ResetFrameVariables()
end

--[FUNCTION]

CreateFSSelectCharacterFrame = function()
	local l_Frame = CreateFSRAFNotificationFrame("FSSelectCharacterFrame", "CENTER", 0,0)
	l_Frame:SetSize(250, 100)
	l_Frame:SetPoint("BOTTOM", 0, 150)

	local l_Title = l_Frame:CreateFontString(nil, 'OVERLAY', "GameFontNormal")
	l_Title:SetText("Activation")
	l_Title:SetJustifyH("CENTER")
	l_Title:SetSize(300, 50)
	l_Title:SetPoint("CENTER", 0, 40)

	local l_Button = CreateFrame("Button", nil, l_Frame, "UIPanelButtonTemplate")
	l_Button:SetText("Activate")
	l_Button:SetSize(80, 30)
	l_Button:SetPoint("BOTTOM", 0, 8)
	l_Button:SetScript("OnClick", function() StaticPopup_Show("CONFIRM_ACTIVATE_RAF") end)

	l_Frame:Hide();
end

--[FUNCTION]

CreateRAFInviteButton = function(p_Name, p_Parent)
	local l_Button = CreateFrame("Button", p_Name, p_Parent, "UIPanelButtonTemplate")
	l_Button:SetSize(70, 23)
	l_Button:SetPoint("RIGHT", -50, 0)
	l_Button:SetScript("OnClick", function() FSRAFInviteButton_OnClick(l_Button) end)
	l_Button:SetText(FS_RAF_INVITE)
end

--[FUNCTION]

CreateFSRAFPlayerNotificationFrame = function(p_Name, p_Parent)
	local l_Frame = CreateFrame("Frame", p_Name, p_Parent)
	local l_Texture = l_Frame:CreateTexture(nil, 'BORDER', nil, -1)
	l_Texture:SetAtlas("UI-Character-Info-ItemLevel-Bounce", "true")
	l_Texture:SetAlpha(0.3)
	l_Texture:SetPoint("CENTER")
	return l_Frame
end

--[FUNCTION]

CreateFSRAFNotificationFrame = function(p_Name, p_Point, p_X, p_Y)
	local l_Frame = CreateFrame("Frame", p_Name, UIParent, "PortraitFrameTemplate")
	l_Frame.Icon = l_Frame:CreateTexture(nil, 'OVERLAY', nil, -1)
	l_Frame.Icon:SetSize(64, 64)
	l_Frame.Icon:SetTexture("Interface\\AddOns\\Firestorm\\assets\\raf_circle.tga")
	l_Frame.Icon:SetPoint("TOPLEFT", -7, 10)

	l_Frame:SetPoint(p_Point, p_X, p_Y)
	l_Frame:SetScript("OnHide", function(self) FSRAFNotification_OnClose(self) end)
	return l_Frame
end

--[FUNCTION]

CreateFSRAFRewardButton = function()
	local l_Frame = CreateFrame("Button", "FSRAFRewardButton", _G["FriendsTabHeader"])
	l_Frame:SetSize(26, 26)
	l_Frame:SetPoint("TOPRIGHT", -35, -55)

	local l_Icon = l_Frame:CreateTexture("FSRAFRewardButtonIcon", "BACKGROUND")
	l_Icon:SetSize(26, 26)
	l_Icon:SetPoint("CENTER")
	l_Icon:SetTexture("Interface\\Icons\\achievement_guildperk_mobilebanking")

	l_Frame:SetPushedTexture("Interface\\Buttons\\UI-Quickslot-Depress")
	l_Frame:SetHighlightTexture("Interface\\Buttons\\ButtonHilight-Square", "ADD")

	l_Frame:SetScript("OnClick", function(self) FSRAFRewardButton_OnClick(self) end)
	l_Frame:SetScript("OnEnter", function(self) FSRAFRewardButton_OnEnter(self) end)
	l_Frame:SetScript("OnLeave", function(self) FSRAFRewardButton_OnLeave(self) end)
end

--[FUNCTION]

FSRAFRewardButton_OnClick = function(self)
	_G["FSRAFReward"]:Show()
end

--[FUNCTION]

FSRAFRewardButton_OnEnter = function(self)
	_G["GameTooltip"]:SetOwner(self, "ANCHOR_RIGHT")
	_G["GameTooltip"]:SetText(RAF_BUTTON_REWARD_TOOLTIP_TITLE)
	_G["GameTooltip"]:AddLine(RAF_BUTTON_REWARD_TOOLTIP_DESCRIPTION, 1, 1, 1, 1)
	_G["GameTooltip"]:Show()
end

--[FUNCTION]

FSRAFRewardButton_OnLeave = function(self)
	_G["GameTooltip"]:Hide();
end

--[FUNCTION]

CreateFSRAFRewardFrame = function()
	local l_Frame = CreateFrame("Frame", "FSRAFReward", UIParent, "PortraitFrameTemplate")
	l_Frame:SetPoint("CENTER", -5, 25)
	l_Frame:SetSize(800, 572)
	l_Frame:Hide()
	l_Frame:SetScript("OnShow", function(self) _G["FSRecruitAFriendSentFrame"]:Hide() _G["FSRecruitAFriendFrame"]:Hide() end)

	local l_Icon = l_Frame:CreateTexture(nil, 'OVERLAY', nil, -1)
	l_Icon:SetSize(64, 64)
	l_Icon:SetTexture("Interface\\AddOns\\Firestorm\\assets\\raf_reward_circle.tga")
	l_Icon:SetPoint("TOPLEFT", -7, 10)

	local l_Title = l_Frame:CreateFontString(nil, 'ARTWORK', "GameFontNormal")
	l_Title:SetText(RECRUIT_A_FRIEND_REWARDS)
	l_Title:SetPoint("TOP", 0, -5)

	l_Frame.SubTitle = l_Frame:CreateFontString(nil, "ARTWORK", "GameFontNormalHuge3")
	l_Frame.SubTitle:SetText(CHOOSE_ONE_REWARD)
	l_Frame.SubTitle:SetPoint("TOP", 0, -35)

	l_Frame.Inset = CreateFrame("Frame", "FSRAFRewardInset", l_Frame, "InsetFrameTemplate")
	l_Frame.Inset:SetPoint("BOTTOMRIGHT", -8, 3)
	l_Frame.Inset:SetPoint("TOPLEFT", 3, -68)

	local l_Button = CreateFrame("Button", "FSRAFRewardClaimReward", l_Frame.Inset, "UIPanelButtonTemplate")
	l_Button:SetText(CLAIM_REWARD)
	l_Button:SetSize(155, 29)
	l_Button:SetPoint("BOTTOM", 0, 18)
	l_Button:SetScript("OnClick", function(self) FSRAFRewardClaimReward_OnClick(self) end)

	FSRAFRewardFrame_SetUp(l_Frame)
end

--[FUNCTION]

FSRAFRewardFrame_SetUp = function(self)
	self.selectedData = nil

	for i=1, 10 do
		local data = FS_RAF_PRODUCTS[i]
		local button = _G["FSRAFRewardChoice"..i]
		if ( not button ) then
			button = CreateFrame("CheckButton", "FSRAFRewardChoice"..i, self.Inset, "ProductChoiceItemTemplate")
			button.id = i
			button:SetScript("OnClick", function(self) FSRAFRewardChoice_OnClick(self) end)
			if ( (i % 10) == 1 ) then
				button:SetPoint("TOPLEFT", self.Inset, "TOPLEFT", 10, -10)
			elseif ( (i % 10) == 6) then
				button:SetPoint("TOPLEFT", _G["FSRAFRewardChoice"..1], "BOTTOMLEFT", 0, -10)
			else
				button:SetPoint("TOPLEFT", _G["FSRAFRewardChoice"..(i-1)], "TOPRIGHT", 10, 0)
			end
		end

		if ( not data ) then
			button:Hide()
		else
			data.modelSceneID = 10
			button.data = data
			button.ModelScene.PreviewButton:Show()

			FSRAFRewardFrameItem_SetUpDisplay(button, data, forceUpdate)
			button:Show()
		end
	end
end

--[FUNCTION]

FSRAFRewardFrameItem_SetUpDisplay = function(self, data)
	self.Name:SetText(data.name);
	if ( data.subtitle ) then
		self.SubTitle:SetFormattedText(PRODUCT_CHOICE_SUBTEXT, data.subtitle);
		self.SubTitle:Show();
	else
		self.SubTitle:Hide();
	end
	self.Covers.CheckMark:SetShown(data.alreadyHas);
	self.Covers.Disabled:SetShown(data.disabled);
	if ( data.modelDisplayID ) then
		self.ModelScene:SetFromModelSceneID(data.modelSceneID, true);

		local item = self.ModelScene:GetActorByTag("item");
		if ( item ) then
			item:SetModelByCreatureDisplayID(data.modelDisplayID);
			item:SetAnimationBlendOperation(LE_MODEL_BLEND_OPERATION_NONE);
		end
		
		self.ModelScene:Show();
		self.Shadow:Show();
		self.Icon:Hide();
		self.IconBorder:Hide();
	else
		self.Shadow:Hide();
		self.ModelScene:Hide();
		self.Shadow:Hide();
		SetPortraitToTexture(self.Icon, data.textureName);
		self.Icon:Show();
		self.IconBorder:Show();
	end
end

--[FUNCTION]

FSRAFRewardChoice_OnClick = function(self)
	for i = 1,10 do
		if (i ~= self.id) then
			_G["FSRAFRewardChoice"..i]:SetChecked(false)
		end
	end

	if (_G["FSRAFReward"].selectedData ~= self.id) then
		_G["FSRAFReward"].selectedData = self.id
	else
		_G["FSRAFReward"].selectedData = 0
	end
end

--[FUNCTION]

FSRAFRewardClaimReward_OnClick = function(self)
	if (_G["FSRAFReward"].selectedData ~= 0) then
		SendChatMessage(".raf fscselectreward ".. _G["FSRAFReward"].selectedData, "guild", nil, nil)
	end
end

--[FUNCTION]

CreateFirestormOptionsFrame = function()
	local l_Frame = CreateFSRAFNotificationFrame("FirestormOptions", "CENTER", 0, 60)
	l_Frame.Icon:SetTexture("Interface\\AddOns\\Firestorm\\assets\\f.tga")
	l_Frame:SetSize(320, 100)
	l_Frame:Hide()
	tinsert(UISpecialFrames, l_Frame:GetName());

	SetRAFFrameTitle(l_Frame, FS_FIRESTORM_OPTIONS)

	l_Frame.RafNotificationButton = CreateFrame("CheckButton", nil, l_Frame, "UIRadioButtonTemplate")
	l_Frame.RafNotificationButton:SetPoint("TOPLEFT", 25, -60)
	l_Frame.RafNotificationButton:SetSize(25, 25)
	l_Frame.RafNotificationButton:SetScript("OnClick", function(self) RafNotificationButton_OnClick(self) end)
	l_Frame.RafNotificationButton:SetChecked(g_NotificationsEnabled)

	l_Frame.RafNotificationText = l_Frame:CreateFontString(nil, "OVERLAY", "GameFontNormal")
	l_Frame.RafNotificationText:SetText(FS_ENABLE_RAF_NOTIFICATION)
	l_Frame.RafNotificationText:SetSize(260, 1)
	l_Frame.RafNotificationText:SetPoint("LEFT", l_Frame.RafNotificationButton, "RIGHT", -15, 0)
end

--[FUNCTION]

RafNotificationButton_OnClick = function(self)
	if (self:GetChecked()) then
		g_NotificationsEnabled = true
		SendChatMessage(".raf enableNotification true", "guild", nil, nil);
	else
		g_NotificationsEnabled = false
		SendChatMessage(".raf enableNotification false", "guild", nil, nil);
	end
end

--[FUNCTION]

FSRecruitAFriend_OnLoad = function(self)
	self.exclusive = true;
	self.hideOnEscape = true;
	self:RegisterEvent("RECRUIT_A_FRIEND_INVITER_FRIEND_ADDED");
	self:RegisterEvent("RECRUIT_A_FRIEND_INVITATION_FAILED");
	ChatFrame_AddMessageEventFilter("CHAT_MSG_SYSTEM", FSRecruitAFriend_OnCustomMessage);
end

--[FUNCTION]

BuffFrame_Update = function()
	BUFF_ACTUAL_DISPLAY = 0;
	for i=1, BUFF_MAX_DISPLAY do
		if ( AuraButton_Update("BuffButton", i, "HELPFUL") ) then
			BUFF_ACTUAL_DISPLAY = BUFF_ACTUAL_DISPLAY + 1;
			ChangeBuffDisplayIfNeeded(i)
		end
	end
	
	DEBUFF_ACTUAL_DISPLAY = 0;
	for i=1, DEBUFF_MAX_DISPLAY do
		if ( AuraButton_Update("DebuffButton", i, "HARMFUL") ) then
			DEBUFF_ACTUAL_DISPLAY = DEBUFF_ACTUAL_DISPLAY + 1;
		end
	end
	
	BuffFrame_UpdateAllBuffAnchors();
end

--[FUNCTION]

ChangeBuffDisplayIfNeeded = function(p_Index)
	local name = UnitAura(PlayerFrame.unit, p_Index, "HELPFULL")
	if (name == "Recruit A Friend !") then
		_G["BuffButton"..p_Index.."Icon"]:SetTexture("Interface\\Icons\\Raf-Icon")
	end
end

--[FUNCTION]

RAFButton_Update = function(self)
    self:Show();
    self.Icon:SetTexture("Interface\\Icons\\Raf-Icon");
end

--[FUNCTION]

SetReferer = function(p_Name)
	if (_G["Referer"] == nil) then
		_G["Referer"] = CreateFSRAFNotificationFrame("Referer", "BOTTOMLEFT", 50, 250);
		SetRAFFrameTitle(_G["Referer"], FS_RAF_REFERER);
	end

	if (_G["Recruits"] ~= nil) then
		_G["Recruits"]:SetPoint("BOTTOMLEFT", 50, 365)
	end

	if (g_Referer ~= "") then
		_G["Referer"]:Show()
		return
	end

	AddPlayer(_G["Referer"], p_Name, 30)
	g_Referer = p_Name;
end

--[FUNCTION]

AddRecruit = function(p_Name)
	if (_G["Recruits"] == nil) then
		_G["Recruits"] = CreateFSRAFNotificationFrame("Recruits", "BOTTOMLEFT", 50, 250)
		SetRAFFrameTitle(_G["Recruits"], FS_RAF_RECRUITS)
	end

    local l_Offset = 0
	if (_G["Referer"] ~= nil) then
		if (_G["Referer"]:IsShown()) then
			l_Offset = l_Offset + 115
		end
	end
	
	_G["Recruits"]:SetPoint("BOTTOMLEFT", 50, 250 + l_Offset)

	g_Recruits[g_RecruitIndex] = p_Name
	g_RecruitFrames[g_RecruitOffset] = AddPlayer(_G["Recruits"], p_Name, 30 * g_RecruitOffset)
	g_RecruitIndex = g_RecruitIndex + 1
	g_RecruitOffset = g_RecruitOffset + 1
end

--[FUNCTION]

RemoveRecruit = function(p_Name)
	if (g_RecruitIndex > 1) then
		for i=1,g_RecruitIndex do
			if (g_Recruits[i] == l_Data[2]) then
				g_Recruits[i] =""
				HideUIPanel(g_RecruitFrames[i])
			end
		end
	end
end

--[FUNCTION]

SetRAFFrameTitle = function(p_Frame, p_Title)
	p_Frame.Title = p_Frame:CreateFontString(nil, 'OVERLAY', "GameFontNormal");
	p_Frame.Title:SetSize(250, 16)
	p_Frame.Title:SetPoint("TOP", 14, -3)
	p_Frame.Title:SetText(p_Title);
end

--[FUNCTION]

AddPlayer = function(p_Frame, p_Name, p_Offset)
	p_Frame:SetSize(210, 70 + p_Offset)
	local l_PlayerFrame = CreateFSRAFPlayerNotificationFrame(nil, p_Frame)
	l_PlayerFrame:SetSize(210, 15)
	l_PlayerFrame:SetPoint("BOTTOMLEFT", p_Frame, "BOTTOMLEFT", 30, p_Offset - 15)
 	local l_PlayerName = l_PlayerFrame:CreateFontString(nil, 'OVERLAY', "GameFontNormal")
	l_PlayerName:SetTextColor(1,1,1);
	l_PlayerName:SetJustifyH("LEFT");
	l_PlayerName:SetSize(100, 16)
	l_PlayerName:SetPoint("LEFT", 0, 0)
	l_PlayerName:SetText(p_Name);
	CreateRAFInviteButton(p_Name, l_PlayerFrame)
	p_Frame:Show()
	return l_PlayerFrame
end

--[FUNCTION]

FSRAFInviteButton_OnClick = function(self)
	InviteUnit(self:GetName())
end

--[FUNCTION]

FSRAFNotification_OnClose = function(self)
	self:Hide()
	if (self == _G["Referer"]) then
		if (_G["Recruits"] ~= nil) then
			_G["Recruits"]:SetPoint("BOTTOMLEFT", 50, 250)
		end
		HideUIPanel(g_Referer .. "_Frame")
		g_Referer = ""
	end

	if (self == _G["Recruits"]) then
		for i=1,g_RecruitIndex do
			HideUIPanel(g_RecruitFrames[i])
		end
		_G["Recruits"]:SetSize(210, 70)
		g_RecruitOffset = 1
	end
end

--[FUNCTION]

StaticPopupSpecial_Show = function(frame)
    if (frame:GetName() == "RecruitAFriendFrame") then
        frame = FSRecruitAFriendFrame
    end

	if ( frame.exclusive ) then
		StaticPopup_HideExclusive();
	end
	StaticPopup_SetUpPosition(frame);
	frame:Show();
end

--[FUNCTION]

SelectCharacter = function()
	SendChatMessage(".raf FSCselectchar", "guild", nil, nil);
	HideUIPanel(FSSelectCharacterFrame);
end

--[FUNCTION]

FSRecruitAFriend_OnCustomMessage = function(p_Self, p_Event, p_Message)
	l_Data 		= ExtractData(p_Message);
	l_Opcode 	= l_Data[1];

	if (l_Opcode == "FSC_RAF_AVAILABLE") then
		ShowUIPanel(FSSelectCharacterFrame);
		return true; 
	end

	if (l_Opcode == "FSC_RAF_RECRUIT_LOGGEDIN") then
		local l_Found = false
		if (g_RecruitIndex > 1) then
			for i=1,g_RecruitIndex do
				if (g_Recruits[i] == l_Data[2]) then
					l_Found = true
				end
			end
		end

		if (l_Found) then
			return true
		end

		AddRecruit(l_Data[2])
		return true; 
	end

	if (l_Opcode == "FSC_RAF_RECRUIT_LOGGEDOUT") then
		RemoveRecruit(l_Data[2])
		return true; 
	end

	if (l_Opcode == "FSC_RAF_REFERER_LOGGEDIN") then
		SetReferer(l_Data[2])
		return true; 
	end

	if (l_Opcode == "FSC_RAF_RECRUIT_ACTIVATED") then
		return true; 
	end

	return false;
end

--[FUNCTION]

FSRefererLoggedInNotification_OnShow = function(self)
	FSRefererLoggedInNotification.Text:SetText("Referer Loggedin " .. g_Referer);
end

--[FUNCTION]

FSRecruitLoggedInNotification_OnShow =function(self)
	FSRecruitLoggedInNotification.Text:SetText("Recruit Loggedin " .. g_Recruit);
end

--[FUNCTION]

FSRecruitAFriend_OnShow = function(self)
	PlaySound(SOUNDKIT.IG_CHARACTER_INFO_OPEN);

	local factionGroup, factionName = UnitFactionGroup("player");
	
	FSRecruitAFriendNameEditBox:SetText("");
	FSRecruitAFriendNoteEditBox:SetText("");
	FSRecruitAFriendNameEditBox:SetFocus();
	
	_G["FSRAFReward"]:Hide()
	_G["FSRecruitAFriendSentFrame"]:Hide()
end

--[FUNCTION]

FSRecruitAFriend_OnEmailTextChanged = function(self, userInput)
	local text = self:GetText();
	local enableSendButton;
	if ( text ~= "" ) then
		_G["FSRecruitAFriendNameEditBoxFill"]:Hide();
		enableSendButton = string.find(text, "@");
	else
		_G["FSRecruitAFriendNameEditBoxFill"]:Show();
	end
	_G["FSRecruitAFriendFrameSendButton"]:SetEnabled(enableSendButton);
end

--[FUNCTION]

FSRecruitAFriend_OnNoteChanged = function(self, userInput)
	local text = self:GetText();
	_G["FSRecruitAFriendNoteEditBoxFill"]:SetShown(text == "");
end

--[FUNCTION]

SendRecruitAFriendRequest = function()
	local l_Email = FSRecruitAFriendNameEditBox:GetText();
	local l_Note  = FSRecruitAFriendNoteEditBox:GetText();
	l_Note = string.gsub(l_Note, "\n", "$$n");


	local l_LenLimit = 185;

	local l_Words = {}
	for l_CurrentWord in l_Note:gmatch("[^%s]+") do
		table.insert(l_Words, l_CurrentWord);
	end

	local l_Lines = {""};
	for l_CurrentWordI,l_CurrentWord in ipairs(l_Words) do
		if (#l_Lines[#l_Lines] + #l_CurrentWord > l_LenLimit) then
			l_Lines[#l_Lines] = l_Lines[#l_Lines]:sub(1, -2)
			table.insert(l_Lines, "")
		end
		l_Lines[#l_Lines] = l_Lines[#l_Lines] .. l_CurrentWord .. " "
	end

	SendChatMessage(".raf FSCbegin " .. l_Email, "guild", nil, nil);

	for l_CurrentLineI, l_CurrentLine in ipairs(l_Lines) do
		SendChatMessage(".raf FSCappendNote " .. l_CurrentLine, "guild", nil, nil);
	end

	SendChatMessage(".raf FSCend", "guild", nil, nil);
end

--[FUNCTION]

FSRecruitAFriend_Send = function()

	SendRecruitAFriendRequest()
	StaticPopupSpecial_Replace(FSRecruitAFriendSentFrame, FSRecruitAFriendFrame);

	StaticPopupSpecial_Show(FSRecruitAFriendSentFrame);
end

--[FUNCTION]

FSRecruitAFriend_ShowInfoDialog = function(dialog, text, showOKButton)
	dialog.Text:SetText(text);
	dialog.OkayButton:SetShown(showOKButton);
	dialog:Show();
end

--[FUNCTION]

FSRecruitAFriend_SetInfoDialogDirection = function(dialog, direction)
	local orientation, offset, point, relativePoint;
	if ( direction == "left" ) then
		orientation = 90;
		offset = 3;
		point = "RIGHT";
		relativePoint = "LEFT";
	elseif ( direction == "right" ) then
		orientation = 270;
		offset = -3;
		point = "LEFT";
		relativePoint = "RIGHT";
	end
	SetClampedTextureRotation(dialog.ArrowShadow, orientation);
	SetClampedTextureRotation(dialog.Arrow, orientation);
	SetClampedTextureRotation(dialog.ArrowGlow, orientation);
	dialog.ArrowShadow:ClearAllPoints()
	dialog.Arrow:ClearAllPoints()
	dialog.ArrowGlow:ClearAllPoints()
	dialog.ArrowShadow:SetPoint(point, dialog, relativePoint, offset, 0);
	dialog.Arrow:SetPoint(point, dialog, relativePoint, offset, 0);
	dialog.ArrowGlow:SetPoint(point, dialog, relativePoint, offset, 0);
end

--[FUNCTION]

MoreDetails_OnClick = function(self)
	local l_Parent = self:GetParent();
	if (self:GetText() == FS_RAF_MORE_DETAILS) then
		l_Parent:SetSize(485, 630)
		ShowUIPanel(FSRecruitAFriendBenefitsFrame)
		self:SetText(FS_RAF_LESS_DETAILS)
	else
		l_Parent:SetSize(485, 265)
		HideUIPanel(FSRecruitAFriendBenefitsFrame)
		self:SetText(FS_RAF_MORE_DETAILS)
	end
end

--[FUNCTION]

FSRecruitAFriend_OnHide = function(self)
	self:SetSize(485, 265)
	HideUIPanel(FSRecruitAFriendBenefitsFrame)
	_G["MoreDetails"]:SetText(FS_RAF_MORE_DETAILS)
	HideUIPanel(FSRecruitAFriendBenefitsFrame)
end

--[FUNCTION]

AddRAFBenefit = function(p_Title, p_Description)
	g_BenefitCount = g_BenefitCount + 1
	local l_Frame = _G["FSRecruitAFriendDetails"]
	local l_Title = l_Frame:CreateFontString("DetailBenefit"..g_BenefitCount, 'ARTWORK', "GameFontHighlightHuge")
	l_Title:SetSize(190, 0)
	l_Title:SetText(p_Title)
	l_Title:SetJustifyH("LEFT")

	if (g_BenefitCount == 1) then
		l_Title:SetPoint("TOPLEFT", 30, -10)
	elseif (g_BenefitCount == 2) then
		l_Title:SetPoint("TOPRIGHT", -26, -10)
	else
		l_Title:SetPoint("TOPLEFT", _G["DetailBenefit"..(g_BenefitCount - 2).."Description"], "BOTTOMLEFT", 0, -20)
	end

	local l_Icon = l_Frame:CreateTexture("DetailBenefit"..g_BenefitCount.."Icon", 'ARTWORK')
	l_Icon:SetSize(16, 16)
	l_Icon:SetTexture("Interface\\Scenarios\\ScenarioIcon-Combat")
	l_Icon:SetPoint("TOPRIGHT", _G["DetailBenefit"..g_BenefitCount], "TOPLEFT", -5, -2)

	local l_Description = l_Frame:CreateFontString("DetailBenefit"..g_BenefitCount.."Description", 'ARTWORK', "GameFontHighlight")
	l_Description:SetSize(190, 0)
	l_Description:SetText(p_Description)
	l_Description:SetJustifyH("LEFT")
	l_Description:SetPoint("TOPLEFT", _G["DetailBenefit"..g_BenefitCount], "BOTTOMLEFT", 0, -10)
end

--[FUNCTION]

Start = function()
	SetVariables()
	InitAllLocales()
	InitRewards()
	CreateFrames()
	_G["GameMenuFrame"]:SetScript("OnShow", function(self) FSGameMenuFrame_OnShow(self) end)
end 

--[FUNCTION]

CreateFrames = function()
	_G["FriendsTabHeaderRecruitAFriendButton"]:Show();
	_G["FriendsTabHeaderRecruitAFriendButton"].Icon:SetTexture("Interface\\Icons\\Raf-Icon");

	CreateFSSelectCharacterFrame()
	CreateMainRAFFrame()
	CreateRecruitAFriendSentFrame()
	CreateFSRAFRewardButton()
	CreateFSRAFRewardFrame()
	CreateFirestormOptionsFrame()

	CreateSelectLanguageFrame()
end

--[FUNCTION]

InitAllLocales = function()
	Raf_InitLocales()
	BenefitPreview_InitLocales()
	BenefitDescriptions_A_InitLocales()
	BenefitDescriptions_B_InitLocales()
	SplashFrame_InitLocales()
end

--[FUNCTION]

Raf_InitLocales = function()
	FS_RAF_MORE_DETAILS				= 400000
	FS_RAF_REFERER					= 400001
	FS_RAF_RECRUITS					= 400002
	FS_RAF_INVITE					= 400003
	FS_RAF_LESS_DETAILS				= 400004
	FS_NO_REWARD_AVAILABLE			= 400005
	FS_FIRESTORM_OPTIONS			= 400006
	FS_ENABLE_RAF_NOTIFICATION		= 400007
end

--[FUNCTION]

BenefitPreview_InitLocales = function()
	FS_BENEFIT_PREVIEW_EXPERIENCE	= 400091
	FS_BENEFIT_PREVIEW_AP			= 400092
	FS_BENEFIT_PREVIEW_NETHERSHARDS	= 400093
	FS_BENEFIT_PREVIEW_LOOT			= 400094
	FS_BENEFIT_PREVIEW_HONOR		= 400095
	FS_BENEFIT_PREVIEW_LEGENDARY	= 400096
end

--[FUNCTION]

BenefitDescriptions_A_InitLocales = function()
	FS_BENEFIT_TITLE_EXPERIENCE		= 400102
	FS_BENEFIT_DESC_EXPERIENCE		= 400103
	FS_BENEFIT_TITLE_AP				= 400104
	FS_BENEFIT_DESC_AP				= 400105
	FS_BENEFIT_TITLE_NETHERSHARDS	= 400106
	FS_BENEFIT_DESC_NETHERSHARDS	= 400107
	FS_BENEFIT_TITLE_LOOT			= 400108
	FS_BENEFIT_DESC_LOOT			= 400109
end

--[FUNCTION]

BenefitDescriptions_B_InitLocales = function()
	FS_BENEFIT_TITLE_HONOR			= 400110
	FS_BENEFIT_DESC_HONOR			= 400111
	FS_BENEFIT_TITLE_LEGENDARY		= 400112
	FS_BENEFIT_DESC_LEGENDARY		= 400113
	FS_BENEFIT_TITLE_TELEPORT		= 400114
	FS_BENEFIT_DESC_TELEPORT		= 400115
	FS_BENEFIT_TITLE_REWARDS		= 400118
	FS_BENEFIT_DESC_REWARDS			= 400119
end

--[FUNCTION]

RedrawRewards = function()
	InitRewards()
	FSRAFRewardFrame_SetUp(_G["FSRAFReward"])

	for i = 1,10 do
		_G["FSRAFRewardChoice"..i]:SetChecked(false)
	end

	_G["FSRAFReward"].selectedData = 0
end

--[FUNCTION]

SetRewardCount = function(p_Count)
	g_RewardCount = p_Count
	if (p_Count > 0) then
		_G["FSRAFRewardButton"]:Show()
		_G["FSRAFReward"].SubTitle:SetText(CHOOSE_ONE_REWARD)		
	else
		_G["FSRAFRewardButton"]:Hide()
		_G["FSRAFReward"].SubTitle:SetText(FS_NO_REWARD_AVAILABLE)
	end
end

--[FUNCTION]

FSGameMenuFrame_OnShow = function(self)
	UpdateMicroButtons();
	Disable_BagButtons();
	VoiceChat_Toggle();

	GameMenuFrame_UpdateVisibleButtons(self);
	AddGameMenuFirestormButton()
end

--[FUNCTION]

AddGameMenuFirestormButton = function()
	_G["GameMenuFrame"]:SetSize(195, 328)
	_G["GameMenuButtonHelp"]:SetPoint("CENTER", _G["GameMenuFrame"], "TOP", 0, -64)

	if (g_FirestormButton == nil) then
		g_FirestormButton = CreateFrame("Button", "GameMenuFirestorm", _G["GameMenuFrame"], "GameMenuButtonTemplate")
		g_FirestormButton:SetText("Firestorm")
		g_FirestormButton:SetScript("OnClick", function(self) GameMenuFirestorm_OnClick(self) end)
	end
	g_FirestormButton:SetPoint("CENTER", _G["GameMenuFrame"], "TOP", 0, -42)
	g_FirestormButton:Show()
end

--[FUNCTION]

GameMenuFirestorm_OnClick = function(self)
	HideUIPanel(_G["GameMenuFrame"])
	_G["FirestormOptions"]:Show()
end

--[FUNCTION]

SetNotificationsEnabled = function(p_Apply)
	g_NotificationsEnabled = p_Apply
	_G["FirestormOptions"].RafNotificationButton:SetChecked(p_Apply)
end

--[FUNCTION]

CanSummonFriend = function()
	return true
end

--[FUNCTION]

SummonFriend = function(unit)
	name = UnitName(unit)
	SendChatMessage(".raf FSCSummon " .. name, "guild", nil, nil);
end

--[FUNCTION]

FSHelpFrameOpenTicketCancel_OnClick = function()
	SendChatMessage(".ticket FSCticketGet", "guild", nil, nil);
	if haveTicket then
		SendChatMessage(".ticket fscdelete", "guild", nil, nil);
	else
		FSHelpFrame:Hide()
		FSHelpFrame_ShowFrame(HELPFRAME_OPEN_TICKET);
	end
end

--[FUNCTION]

CreateSelectLanguageFrame = function()
	_G["FSHelpFrameTicketSubmit"]:SetScript("OnClick", FSHelpFrameTicketSubmit_OnClick)

	g_Frame = CreateFrame("Frame", "SelectLanguageFrame", _G["FSHelpFrameTicketSubmit"], "TranslucentFrameTemplate")
	g_Frame:SetSize(250, 150)
	g_Frame:SetPoint("CENTER", 100, 275)

	g_Frame:Hide()

	g_Layer = g_Frame:CreateFontString(nil, "OVERLAY", "GameFontHighlight")
	g_Layer:SetText("Choose your language")
	g_Layer:SetPoint("TOP", 0, -22)

	CreateLanguageButton("English", 0)
	CreateLanguageButton("French", 2)
	CreateLanguageButton("Spanish", 6)
	CreateLanguageButton("Portuguese", 9)
	CreateLanguageButton("Russian", 8)

	ResetFrameVariables()
end

--[FUNCTION]

CreateLanguageButton = function(p_LanguageName, p_LanguageID)
	g_SubFrame = CreateFrame("Button", "LanguageButton"..g_CurrentLanguageButton, _G["SelectLanguageFrame"], "UIPanelButtonTemplate")
	g_SubFrame:SetText(p_LanguageName)
	g_SubFrame:SetSize(100, 22)
	g_SubFrame:SetScript("OnClick", function(self) SetTicketLanguage(p_LanguageID) end)

	if (g_CurrentLanguageButton == 0) then
		g_SubFrame:SetPoint("TOPLEFT", 18, -45)
		g_CurrentLanguageButton = g_CurrentLanguageButton + 1
		return
	end

	if (g_CurrentLanguageButton == 1) then
		g_SubFrame:SetPoint("TOPRIGHT", -18, -45)
		g_CurrentLanguageButton = g_CurrentLanguageButton + 1
		return
	end

	g_SubFrame:SetPoint("TOP", _G["LanguageButton"..(g_CurrentLanguageButton - 2)], "BOTTOM", 0, -8)
	g_CurrentLanguageButton = g_CurrentLanguageButton + 1
end

--[FUNCTION]

SetTicketLanguage = function(p_Language)
	g_Language = p_Language
	FSHelpFrameOpenTicketSubmit_OnClick()
	_G["SelectLanguageFrame"]:Hide()
	SendChatMessage(".ticket FSCSetLang " .. g_Language, "guild", nil, nil);
end

--[FUNCTION]

FSHelpFrameTicketSubmit_OnClick = function()
	if (not g_Language) then
		_G["SelectLanguageFrame"]:Show()
		return
	end

	FSHelpFrameOpenTicketSubmit_OnClick()
	SendChatMessage(".ticket FSCSetLang " .. g_Language, "guild", nil, nil);
end

--[FUNCTION]

GenerateSplashFrame = function(date)
	SPLASH_SCREENS["LEGION_CURRENT"].label = FS_SPLASH_FRAME_LAST_CHANGELOG .. ": " .. date
	SPLASH_SCREENS["LEGION_CURRENT"].rightTitle = SPLASH_LEGION_NEW_7_3_RIGHT_TITLE
	SPLASH_SCREENS["LEGION_CURRENT"].rightDesc = SPLASH_LEGION_NEW_7_3_RIGHT_DESC

	SplashFrame_Display("LEGION_CURRENT", false)
	_G["SplashFrame"]:Hide()

	_G["SplashFrame"].RightTexture:SetAtlas("splash-730-right")
	_G["SplashFrame"].Label:SetPoint("TOPLEFT", 55, -50)
	_G["SplashFrame"].Label:SetSize(314, 30)
	_G["SplashFrame"].Label:SetJustifyH("CENTER")

	local CHANGELOG_BACKDROP_TABLE = {
	bgFile = "Interface\\DialogFrame\\UI-DialogBox-Background-Dark",
	edgeFile = "Interface\\DialogFrame\\UI-DialogBox-Gold-Border",
	tile = true,
	tileSize = 32,
	edgeSize = 32,
	insets = { left = 11, right = 12, top = 12, bottom = 11 }};

	local l_Backdrop = CreateFrame("Frame", "Backdrop", _G["SplashFrame"])
	l_Backdrop:SetSize(314, 425)
	l_Backdrop:SetPoint("TOPLEFT", _G["SplashFrame"], "TOPLEFT", 55, -95)
	l_Backdrop:SetFrameStrata("DIALOG")
	l_Backdrop:SetFrameLevel(3)
	l_Backdrop:SetBackdrop(CHANGELOG_BACKDROP_TABLE)
	l_Backdrop = CreateFrame("Frame", "Backdrop2", _G["SplashFrame"])
	l_Backdrop:SetSize(314, 425)
	l_Backdrop:SetPoint("TOPLEFT", _G["SplashFrame"], "TOPLEFT", 55, -95)
	l_Backdrop:SetFrameStrata("DIALOG")
	l_Backdrop:SetFrameLevel(3)
	l_Backdrop:SetBackdrop(CHANGELOG_BACKDROP_TABLE)
	l_Backdrop = CreateFrame("Frame", "Backdrop2", _G["SplashFrame"])
	l_Backdrop:SetSize(314, 425)
	l_Backdrop:SetPoint("TOPLEFT", _G["SplashFrame"], "TOPLEFT", 55, -95)
	l_Backdrop:SetFrameStrata("DIALOG")
	l_Backdrop:SetFrameLevel(3)
	l_Backdrop:SetBackdrop(CHANGELOG_BACKDROP_TABLE)

	local l_Inset = CreateFrame("Frame", "SplashFrameInset", _G["SplashFrame"])
	l_Inset:SetSize(271, 415)
	l_Inset:SetPoint("TOPLEFT", _G["SplashFrame"], "TOPLEFT", 65, -100)
	l_Inset:SetFrameStrata("DIALOG")
	l_Inset:SetFrameLevel(2)

	SetBorders(l_Inset, "SplashFrameInset")

	local l_ScrollFrame = CreateFrame("ScrollFrame", "SplashFrameInsetScrollFrame", l_Inset, "UIPanelScrollFrameTemplate")
	l_ScrollFrame:SetPoint("TOPLEFT", _G["SplashFrameInset"], "TOPLEFT", 6, -6)
	l_ScrollFrame:SetPoint("BOTTOMRIGHT", _G["SplashFrameInset"], "BOTTOMRIGHT", 0, 6)

	local l_ScrollChild = CreateFrame("Frame", "SplashFrameInsetScrollChild", l_ScrollFrame)
	l_ScrollChild:SetSize(271, 400)
	l_ScrollFrame:SetScrollChild(l_ScrollChild)

	AttributeNameIcons()
	DrawChangelog(false)
	CreateOnlyPlayerClassButton()
	CreateTabs()
end

--[FUNCTION]

AddChangelogItem = function(p_Title, p_Description)
	g_ChangelogItemCount = g_ChangelogItemCount + 1

	local l_ChangelogItem = {
		Title = p_Title,
		Description = p_Description
	}

	g_ChangelogItems[g_ChangelogItemCount] = l_ChangelogItem
end

--[FUNCTION]

DrawChangelog = function(onlyPlayerClass)
	ResetChangelog()

	local i = 1
	slot = 1
	while i <= g_ChangelogItemCount do
		slot = DrawChangelogItem(g_ChangelogItems[i], onlyPlayerClass, slot)
		i = i + 1
	end
end

--[FUNCTION]

ResetChangelog = function()
	local i = 1
	while _G["ChangelogItem"..i] ~= nil do
		_G["ChangelogItem" .. i]:Hide()
		_G["ChangelogItem" .. i .. "Description"]:Hide()
		local j = 1
		while _G["ChangelogItem" .. i .. "Icon" .. j] do
			_G["ChangelogItem" .. i .. "Icon" .. j]:Hide()
			j = j + 1
		end
		i = i + 1
	end
end

--[FUNCTION]

DrawChangelogItem = function(p_ChangelogItem, onlyPlayerClass, slot)
	l_TitleText, icons, ignore = ParseTitle(p_ChangelogItem.Title, onlyPlayerClass)
	if ignore then
		return slot
	end

	local l_Frame = _G["SplashFrameInsetScrollChild"]

	local l_Title = _G["ChangelogItem"..slot]
	if l_Title == nil then
		l_Title = l_Frame:CreateFontString("ChangelogItem"..slot, 'ARTWORK', "GameFontHighlightHuge")
	end

	l_Title:Show()
	l_Title:SetSize(235, 0)
	l_Title:SetJustifyH("LEFT")

	if (slot == 1) then
		l_Title:SetPoint("TOPLEFT", 30, -10)
	else
		l_Title:SetPoint("TOPLEFT", _G["ChangelogItem"..(slot - 1).."Description"], "BOTTOMLEFT", 0, -20)
	end

	local i = 1
	local l_Offset = 0
	local l_UpperOffset = 0
	while icons[i] do
		local l_Icon = _G["ChangelogItem"..slot.."Icon" .. i]
		if l_Icon == nil then
			l_Icon = l_Frame:CreateTexture("ChangelogItem"..slot.."Icon" .. i, 'ARTWORK')
		end

		l_Icon:Show()
		l_Icon:SetTexture(icons[i].icon)
		l_Icon:SetSize(icons[i].sizeX, icons[i].sizeY)
		l_Icon:SetPoint("TOPLEFT", _G["ChangelogItem"..slot], "TOPLEFT", -28 + l_Offset, icons[i].offsetY)
		l_Offset = l_Offset + icons[i].sizeX + 2 - icons[i].offsetReduction
		if (l_UpperOffset < icons[i].upperOffset) then
			l_UpperOffset = icons[i].upperOffset
		end
		i = i + 1
	end

	if l_UpperOffset ~= 0 then
		if (slot == 1) then
			l_Title:SetPoint("TOPLEFT", 30, -(10 + l_UpperOffset))
		else
			l_Title:SetPoint("TOPLEFT", _G["ChangelogItem"..(slot - 1).."Description"], "BOTTOMLEFT", 0, -(20 + l_UpperOffset))
		end
	end

	i = 1
	while i < (l_Offset / 5) - 5 do
		l_TitleText = " " .. l_TitleText
		i = i + 1
	end

	l_Title:SetText(l_TitleText)

	local l_Description = _G["ChangelogItem"..slot.."Description"]
	if l_Description == nil then
		l_Description = l_Frame:CreateFontString("ChangelogItem"..slot.."Description", 'OVERLAY', "GameFontHighlight")
	end

	l_Description:Show()
	l_Description:SetSize(235, 0)
	l_Description:SetText(p_ChangelogItem.Description)
	l_Description:SetJustifyH("LEFT")
	l_Description:SetPoint("TOPLEFT", _G["ChangelogItem"..slot], "BOTTOMLEFT", 0, -10)

	return slot + 1
end

--[FUNCTION]

ParseTitle = function(p_Title, onlyPlayerClass)
	local ignore = onlyPlayerClass
	local _,playerClass = UnitClass("player");
	local endstr = string.find(p_Title, "%]")
	if endstr ~= nil then
		local itemType = string.sub(p_Title, 2, endstr - 1)

		icons = {}
		iconId = 1

		icons[iconId] = NewIcon()
		if string.upper(itemType) == "MISC" then
			icons[iconId].icon = "Interface\\ICONS\\INV_Misc_Wrench_01"
			p_Title = string.sub(p_Title, string.find(p_Title, "%]") + 1, string.len(p_Title))
			endstr = string.find(p_Title, "%]")

			if endstr ~= nil then
				local category = string.sub(p_Title, 2, endstr - 1)
				if string.upper(category) == "PROFESSION" then
					p_Title = string.sub(p_Title, string.find(p_Title, "%]") + 1, string.len(p_Title))
					endstr = string.find(p_Title, "%]")
					if endstr ~= nil then
						local profession = string.sub(p_Title, 2, endstr - 1)
						icons[iconId].icon = "Interface\\ICONS\\Trade_"..profession
						p_Title = string.sub(p_Title, string.find(p_Title, "%]") + 1, string.len(p_Title))
					end
				elseif string.upper(category) == "QUEST" then
					icons[iconId].icon = "Interface\\ICONS\\INV_Misc_ScrollUnrolled01"
					p_Title = string.sub(p_Title, string.find(p_Title, "%]") + 1, string.len(p_Title))
				end
			end

			iconId = iconId + 1
		elseif string.upper(itemType) == "PVE" then
			p_Title = string.sub(p_Title, string.find(p_Title, "%]") + 1, string.len(p_Title))
			endstr = string.find(p_Title, "%]")

			if endstr ~= nil then
				local bossName = string.sub(p_Title, 2, endstr - 1)
				icons[iconId] = NewIcon()
				icons[iconId].icon = "Interface\\EncounterJournal\\UI-EJ-BOSS-".. bossName
				icons[iconId].sizeX = 80
				icons[iconId].sizeY = 40
				icons[iconId].offsetReduction = 18
				icons[iconId].offsetY = 20
				icons[iconId].upperOffset = 8
				iconId = iconId + 1
				p_Title = string.sub(p_Title, string.find(p_Title, "%]") + 1, string.len(p_Title))
			end
		else
			ignore = onlyPlayerClass and (playerClass ~= string.upper(itemType)) and (string.upper(itemType) ~= "GENERIC")
			icons[iconId].icon = "Interface\\ICONS\\ClassIcon_".. itemType
			iconId = iconId + 1
			p_Title = string.sub(p_Title, string.find(p_Title, "%]") + 1, string.len(p_Title))

			if iconId > 1 then
				while true do
					endstr = string.find(p_Title, "%]")

					if endstr == nil then
						break
					end

					local specialization = string.sub(p_Title, 2, endstr - 1)

					local iconKey = string.upper(itemType .. "_" .. specialization)
					if nameIcon[iconKey] == nil then
						break
					end

					icons[iconId] = NewIcon()
					icons[iconId].icon = "Interface\\ICONS\\".. nameIcon[iconKey]
					iconId = iconId + 1
					p_Title = string.sub(p_Title, string.find(p_Title, "%]") + 1, string.len(p_Title))
				end
			end
		end
	end

	return p_Title, icons, ignore
end

--[FUNCTION]

AttributeNameIcons = function()
	nameIcon = {}
	nameIcon["WARRIOR_ARMS"] = "Ability_Warrior_SavageBlow"
	nameIcon["WARRIOR_FURY"] = "Ability_Warrior_InnerRage"
	nameIcon["WARRIOR_PROTECTION"] = "Ability_Warrior_DefensiveStance"
	nameIcon["PALADIN_HOLY"] = "Spell_Holy_HolyBolt"
	nameIcon["PALADIN_PROTECTION"] = "Ability_Paladin_ShieldOfTheTemplar"
	nameIcon["PALADIN_RETRIBUTION"] = "Spell_Holy_AuraOfLight"
	nameIcon["HUNTER_BEASTMASTERY"] = "Ability_Hunter_BestialDiscipline"
	nameIcon["HUNTER_MARKSMANSHIP"] = "Ability_Hunter_FocusedAim"
	nameIcon["HUNTER_SURVIVAL"] = "Ability_Hunter_Camouflage"
	nameIcon["ROGUE_ASSASSINATION"] = "Ability_Rogue_DeadlyBrew"
	nameIcon["ROGUE_OUTLAW"] = "Ability_Rogue_Waylay"
	nameIcon["ROGUE_SUBTLETY"] = "Ability_Stealth"
	nameIcon["PRIEST_DISCIPLINE"] = "Spell_Holy_PowerWordShield"
	nameIcon["PRIEST_HOLY"] = "Spell_Holy_GuardianSpirit"
	nameIcon["PRIEST_SHADOW"] = "Spell_Shadow_ShadowWordPain"
	nameIcon["DEATHKNIGHT_BLOOD"] = "Spell_DeathKnight_BloodPresence"
	nameIcon["DEATHNKIGHT_FROST"] = "Spell_DeathKnight_FrostPresence"
	nameIcon["DEATHNKIGHT_UNHOLY"] = "Spell_DeathKnight_UnholyPresence"
	nameIcon["SHAMAN_ELEMENTAL"] = "Spell_Nature_Lightning"
	nameIcon["SHAMAN_ENHANCEMENT"] = "Spell_Shaman_ImprovedStormstrike"
	nameIcon["SHAMAN_RESTORATION"] = "Spell_Nature_MagicImmunity"
	nameIcon["MAGE_ARCANE"] = "Spell_Holy_MagicalSentry"
	nameIcon["MAGE_FIRE"] = "Spell_Fire_Firebolt02"
	nameIcon["MAGE_FROST"] = "Spell_Frost_Frostbolt02"
	nameIcon["WARLOCK_AFFLICTION"] = "Spell_Shadow_Deathcoil"
	nameIcon["WARLOCK_DEMONOLOGY"] = "Spell_Shadow_Metamorphosis"
	nameIcon["WARLOCK_DESTRUCTION"] = "Spell_Shadow_RainOfFire"
	nameIcon["MONK_BREWMASTER"] = "Spell_Monk_BrewMaster_Spec"
	nameIcon["MONK_MISTWEAVER"] = "Spell_Monk_MistWeaver_Spec"
	nameIcon["MONK_WINDWALKER"] = "Spell_Monk_WindWalker_Spec"
	nameIcon["DRUID_BALANCE"] = "Spell_Nature_Starfall"
	nameIcon["DRUID_FERAL"] = "Ability_Druid_CatForm"
	nameIcon["DRUID_GUARDIAN"] = "Ability_Racial_BearForm"
	nameIcon["DRUID_RESTORATION"] = "Spell_Nature_HealingTouch"
	nameIcon["DEMONHUNTER_HAVOC"] = "Ability_DemonHunter_SpecDPS"
	nameIcon["DEMONHUNTER_VENGEANCE"] = "Ability_DemonHunter_SpecTank"

end

--[FUNCTION]

NewIcon = function()
	local icon = {
		icon = "",
		sizeX = 25,
		sizeY = 25,
		offsetReduction = 0,
		offsetY = 3,
		upperOffset = 0
	}

	return icon
end

--[FUNCTION]

CreateOnlyPlayerClassButton = function()
	local l_ButtonLabel = _G["SplashFrame"]:CreateFontString("OnlyPlayerClassCheckBoxLabel", 'OVERLAY', "GameFontHighlight")
	l_ButtonLabel:SetPoint("TOPLEFT", _G["SplashFrameInset"], "BOTTOMLEFT", 35, -9)
	l_ButtonLabel:SetSize(271, 20)
	l_ButtonLabel:SetJustifyH("LEFT")
	l_ButtonLabel:SetText(FS_SPLASH_FRAME_ONLY_MY_CLASS)

	local l_Button = CreateFrame("CheckButton", "OnlyPlayerClassCheckBox", _G["SplashFrame"], "InterfaceOptionsCheckButtonTemplate")
	l_Button:SetPoint("TOPLEFT", _G["SplashFrameInset"], "BOTTOMLEFT", 8, -7)
	l_Button:SetFrameStrata("DIALOG")
	l_Button:SetScript("OnClick", function(self)
		DrawChangelog(self:GetChecked())
	end)
end

--[FUNCTION]

CreateTabs = function()
	local l_Header = CreateFrame("Frame", "SplashFrameHeader", _G["SplashFrame"])

	local l_Button1 = CreateFrame("Button", "SplashFrameHeaderTab1", l_Header, "TabButtonTemplate")
	l_Button1:SetPoint("BOTTOMLEFT", _G["SplashFrameInset"], "TOPLEFT", 5, -4)
	l_Button1:SetFrameStrata("DIALOG")
	l_Button1:SetFrameLevel(5)
	l_Button1:SetText(FS_SPLASH_FRAME_CHANGELOG)
	l_Button1:SetID(1)
	PanelTemplates_TabResize(l_Button1, 0)
	l_Button1:SetWidth(l_Button1:GetTextWidth() + 31)
	l_Button1:SetScript("OnClick", function(self)
		PanelTemplates_Tab_OnClick(self, l_Header)
		PanelTemplates_ResizeTabsToFit(l_Header, 270)
		SplashFrame_Update()
	end)

	local l_Button2 = CreateFrame("Button", "SplashFrameHeaderTab2", l_Header, "TabButtonTemplate")
	l_Button2:SetPoint("LEFT", l_Button1, "RIGHT", 0, 0)
	l_Button2:SetFrameStrata("DIALOG")
	l_Button2:SetFrameLevel(5)
	l_Button2:SetText(FS_SPLASH_FRAME_NEWS_FEED)
	l_Button2:SetID(2)
	PanelTemplates_TabResize(l_Button2, 0)
	l_Button2:SetWidth(l_Button2:GetTextWidth() + 31)
	l_Button2:SetScript("OnClick", function(self)
		PanelTemplates_Tab_OnClick(self, l_Header)
		PanelTemplates_ResizeTabsToFit(l_Header, 270)
		SplashFrame_Update()
	end)

	PanelTemplates_SetNumTabs(l_Header, 2)
	PanelTemplates_SetTab(l_Header, 1)
	FriendsTabHeader_ResizeTabs()
end

--[FUNCTION]

SplashFrame_Update = function()
	if _G["SplashFrameHeader"].selectedTab == 1 then
		DrawChangelog()
		_G["OnlyPlayerClassCheckBox"]:Show()
		_G["OnlyPlayerClassCheckBoxLabel"]:Show()

		ResetNewsFeed()
	else
		ResetChangelog()
		_G["OnlyPlayerClassCheckBox"]:Hide()
		_G["OnlyPlayerClassCheckBoxLabel"]:Hide()

		DrawNewsFeed()
	end
end

--[FUNCTION]

DrawNewsFeed = function()
	ResetNewsFeed()
	local i = 1
	while i <= g_NewsFeedItemCount do
		DrawNewsFeedItem(i)
		i = i + 1
	end
end

--[FUNCTION]

DrawNewsFeedItem = function(index)
	local l_Frame = _G["SplashFrameInsetScrollChild"]

	if index > 1 then
		local l_NewsFeedItemSeparator = _G["NewsFeedItemSeparator" .. index]
		if l_NewsFeedItemSeparator == nil then
			l_NewsFeedItemSeparator = l_Frame:CreateTexture("NewsFeedItemSeparator" .. index, 'ARTWORK')
		end

		l_NewsFeedItemSeparator:Show()
		l_NewsFeedItemSeparator:SetTexture("Interface\\HelpFrame\\CS_HelpTextures")
		l_NewsFeedItemSeparator:SetSize(235, 30)
		l_NewsFeedItemSeparator:SetPoint("TOP", _G["NewsFeedItemDescription" .. (index - 1)], "BOTTOM", 0, -2)
		l_NewsFeedItemSeparator:SetTexCoord(0.00195313, 0.44921875, 0.00781250, 0.35937500)
	end

	local l_NewsFeedItemDate = _G["NewsFeedItemDate" .. index]
	if l_NewsFeedItemDate == nil then
		l_NewsFeedItemDate = l_Frame:CreateFontString("NewsFeedItemDate" .. index, 'OVERLAY', "GameFontHighlight")
	end

	l_NewsFeedItemDate:Show()
	l_NewsFeedItemDate:SetSize(235, 0)
	l_NewsFeedItemDate:SetText(g_NewsFeedItems[index].Date)
	l_NewsFeedItemDate:SetJustifyH("LEFT")

	if index == 1 then
		l_NewsFeedItemDate:SetPoint("TOPLEFT", l_Frame, "TOPLEFT", 20, -10)
	else
		l_NewsFeedItemDate:SetPoint("TOPLEFT", _G["NewsFeedItemSeparator" .. index], "BOTTOMLEFT", 0, -2)
	end

	local l_NewsFeedItemIcon = _G["NewsFeedItemIcon" .. index]
	if l_NewsFeedItemIcon == nil then
		l_NewsFeedItemIcon = l_Frame:CreateTexture("NewsFeedItemIcon"..index, 'ARTWORK')
	end

	l_NewsFeedItemIcon:Show()
	l_NewsFeedItemIcon:SetSize(16, 16)
	l_NewsFeedItemIcon:SetTexture("Interface\\Scenarios\\ScenarioIcon-Combat")
	l_NewsFeedItemIcon:SetPoint("TOPRIGHT", _G["NewsFeedItemDate"..index], "TOPLEFT", -4, 1)

	local l_NewsFeedItemDescription = _G["NewsFeedItemDescription" .. index]
	if l_NewsFeedItemDescription == nil then
		l_NewsFeedItemDescription = l_Frame:CreateFontString("NewsFeedItemDescription" .. index, 'OVERLAY', "GameFontHighlight")
	end

	l_NewsFeedItemDescription:Show()
	l_NewsFeedItemDescription:SetSize(235, 0)
	l_NewsFeedItemDescription:SetText(g_NewsFeedItems[index].Text)
	l_NewsFeedItemDescription:SetJustifyH("LEFT")
	l_NewsFeedItemDescription:SetPoint("TOPLEFT", _G["NewsFeedItemDate" .. index], "BOTTOMLEFT", 0, -5)
end

--[FUNCTION]

ResetNewsFeed = function()
	local i = 1
	while _G["NewsFeedItemDate"..i] ~= nil do
		_G["NewsFeedItemDate" .. i]:Hide()
		_G["NewsFeedItemIcon" .. i]:Hide()
		_G["NewsFeedItemDescription" .. i]:Hide()
		if i > 1 then
			_G["NewsFeedItemSeparator" .. i]:Hide()
		end
		i = i + 1
	end
end

--[FUNCTION]

AddNewsFeedItem = function(p_Date, p_Text)
	g_NewsFeedItemCount = g_NewsFeedItemCount + 1
	g_NewsFeedItems[g_NewsFeedItemCount] = {
		Date = p_Date;
		Text = p_Text;
	}
end

--[FUNCTION]

DrawAlertButton = function()
	local l_Header = _G["SplashFrameHeader"]

	l_AlertButtonBack = CreateFrame("Button", "AlertButton", l_Header)

	l_WarningTextureBack = l_AlertButtonBack:CreateTexture("AlertButton", 'OVERLAY')
	l_WarningTextureBack:SetTexture("Interface\\DialogFrame\\UI-Dialog-Icon-AlertNew")
	l_WarningTextureBack:SetAllPoints(true)

	l_AlertButtonBack:SetPoint("BOTTOMRIGHT", _G["SplashFrameInset"], "TOPRIGHT", 15, 5)
	l_AlertButtonBack:SetSize(25, 25)
	l_AlertButtonBack:SetFrameStrata("DIALOG")
	l_AlertButtonBack:SetFrameLevel(4)

	l_AlertButton = CreateFrame("Button", "AlertButton", l_Header)
	l_AlertButton:SetNormalTexture("Interface\\DialogFrame\\UI-Dialog-Icon-AlertNew")

	l_WarningTexture = l_AlertButton:CreateTexture("AlertButton", 'OVERLAY')
	l_WarningTexture:SetTexture("Interface\\DialogFrame\\UI-Dialog-Icon-AlertNew")
	l_WarningTexture:SetAllPoints(true)
	l_WarningTexture:SetBlendMode("ADD")

	l_AlertButtonAnimationGroup = l_AlertButton:CreateAnimationGroup("AlertButtonAnimationGroup")
	l_AlertButtonAnimationGroup:SetLooping("REPEAT")
	l_AlertButtonAnimation1 = l_AlertButtonAnimationGroup:CreateAnimation("Alpha", "PulseOverlay")
	l_AlertButtonAnimation1:SetFromAlpha(0)
	l_AlertButtonAnimation1:SetToAlpha(1)
	l_AlertButtonAnimation1:SetDuration(1)
	l_AlertButtonAnimation1:SetOrder(1)
	l_AlertButtonAnimation2 = l_AlertButtonAnimationGroup:CreateAnimation("Alpha", "PulseOverlay")
	l_AlertButtonAnimation2:SetFromAlpha(1)
	l_AlertButtonAnimation2:SetToAlpha(0)
	l_AlertButtonAnimation2:SetDuration(1)
	l_AlertButtonAnimation2:SetOrder(2)

	l_AlertButtonAnimationGroup:Play()

	l_AlertButton:SetPoint("BOTTOMRIGHT", _G["SplashFrameInset"], "TOPRIGHT", 15, 5)
	l_AlertButton:SetSize(25, 25)
	l_AlertButton:SetFrameStrata("DIALOG")
	l_AlertButton:SetFrameLevel(5)
	l_AlertButton:SetScript("OnEnter", function(self)
		GameTooltip:SetOwner(self, "ANCHOR_RIGHT")
		GameTooltip:SetText(g_AlertButtonText)
		GameTooltip:Show()
	end)
	l_AlertButton:SetScript("OnLeave", function(self)
		if GameTooltip:GetOwner() == self then
			GameTooltip:Hide()
		end
	end)
end

--[FUNCTION]

AddAlert = function(p_Alert)
	local l_AlertButton = _G["AlertButton"]
	if l_AlertButton == nil then
		DrawAlertButton()
	end

	if g_AlertButtonText == nil then
		g_AlertButtonText = p_Alert
	else
		g_AlertButtonText = g_AlertButtonText .. "\n\n" .. p_Alert
	end
end

--[FUNCTION]

SLASH_CHANGELOG1 = "/changelog"
SlashCmdList["CHANGELOG"] = function(msg)
	_G["SplashFrame"]:Show()
	PanelTemplates_Tab_OnClick(_G["SplashFrameHeaderTab1"], _G["SplashFrameHeader"])
	PanelTemplates_ResizeTabsToFit(_G["SplashFrameHeader"], 200)
	SplashFrame_Update()
end

SLASH_NEWS1 = "/news"
SlashCmdList["NEWS"] = function(msg)
	_G["SplashFrame"]:Show()
	PanelTemplates_Tab_OnClick(_G["SplashFrameHeaderTab2"], _G["SplashFrameHeader"])
	PanelTemplates_ResizeTabsToFit(_G["SplashFrameHeader"], 200)
	SplashFrame_Update()
end

--[FUNCTION]

SplashFrame_InitLocales = function()
	FS_SPLASH_FRAME_LAST_CHANGELOG	= 400200
	FS_SPLASH_FRAME_CHANGELOG		= 400201
	FS_SPLASH_FRAME_NEWS_FEED		= 400202
	FS_SPLASH_FRAME_ONLY_MY_CLASS	= 400203
end

--[FUNCTION]
