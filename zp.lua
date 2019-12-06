local ui_parent = require "ui.UI_BaseIcon"
local UI_ZhuanPan = Inherit(ui_parent)
UI_ZhuanPan:DynamicLoad("/Game/UI/UMG/OperationActivity/ZhuanPan.ZhuanPan")
local RoundPhase={
	PhaseType_SpeedUp=1,
	PhaseType_Const=2,
	PhaseType_Decay=3
}
local m_iConstNumPointer  --匀速圈数
local m_fConstTime  	   --匀速时间	
local m_iDecayNumPointer  --减速圈数
local m_iIncreaseNumPointer  --加速圈数


local m_eNowPhasePointer = RoundPhase.PhaseType_Const --指针当前的阶段
local m_fLogicAnglePointer	--指针当前距离槽位0的偏移量

local m_iReward0SlotIdx = 0                                                --旋转前奖品0的槽位索引
local m_iReward0NewSlotIdx = 0                                             --旋转后奖品0的槽位索引
local m_iLastWonRewardSlotIdx = 0                                          --上次中奖奖品的槽位索引
local m_iWonRewardIdx = 0                                                  --本次中奖的奖品索引
local m_iSlotOffsetTurnTable = 0                                           --转盘槽位的偏移量


local m_iSlotNum = 10 --槽位数
local m_fSlotDeltaDegree --单个槽位占的角度
local m_fLastOffSetPointer = 0        --旋转后指针相比初始位置转过的角度
local m_fIncTimePointer         --加速时间        指针
local m_fDecayTimePointer         --减速时间        指针
local m_fAccerPointer             --角加速度        指针
local m_fDecayAnglePointer        --总旋转角度        指针
local m_fVPointer                 --初速度              指针	
local m_fNowTime = 0                  --当前累积旋转时间
local m_fTimeEnterDecayPointer    --进入减速状态的时间点    指针
local m_bIsPlaying = false
local m_drawing = false  --等待服务器消息
local m_UnkhownIndex = 6

local m_SpecialItemID = 10143
function UI_ZhuanPan:Ctor(controller)
	self:BindWidgets()
	self:InitEvent()
	self:InitData()
	self:PreLoad()
end

function UI_ZhuanPan:InitData()
	m_iConstNumPointer = GetDefaultConfig("UI_ZhuanPan")["ConstCircle"]
	m_fConstTime = GetDefaultConfig("UI_ZhuanPan")["ConstTime"]
	m_iDecayNumPointer = GetDefaultConfig("UI_ZhuanPan")["DecayNum"]
	m_iIncreaseNumPointer = GetDefaultConfig("UI_ZhuanPan")["IncreaseNum"]
	m_fSlotDeltaDegree = 360.0/m_iSlotNum
	m_bIsPlaying = false
	m_drawing = false  

	m_fLastOffSetPointer = 0
	m_iLastWonRewardSlotIdx = 0
	m_iWonRewardIdx = 0 
end

function UI_ZhuanPan:PreLoad()
	self.u_WrapBoxTips:ClearChildren()
	self:EnableTick(true)
end

function UI_ZhuanPan:DestroyChildren()
	self.m_TimeList = nil
	self.m_TxtList = nil
	self.m_ShowList = nil
end
--得到需要转过的角度
-------------------------------------------------------------------------------
function UI_ZhuanPan:GetNeedRotateAngle(RoundNum,SlotOffset)
	return RoundNum * 360.0 + SlotOffset * m_fSlotDeltaDegree
end
--计算本次旋转槽位索引的偏移量
-------------------------------------------------------------------------------
function UI_ZhuanPan:GetSlotOffSet(LastSlotIdx,NewSlotIdx)
	local _SlotOffset

	if NewSlotIdx >= LastSlotIdx then
		_SlotOffset = NewSlotIdx - LastSlotIdx
	else
		_SlotOffset = NewSlotIdx + m_iSlotNum - LastSlotIdx
	end
	return _SlotOffset
end
-------------------------------------------------------------------------------
function UI_ZhuanPan:GetWonRewardSlotIdx(Reward0SlotIdx, WonRewardIdx)
	local _RewardIdxOffset = (m_iSlotNum - WonRewardIdx) % m_iSlotNum
	local _WonRewardSlotIdx = (Reward0SlotIdx + _RewardIdxOffset) % m_iSlotNum
	return _WonRewardSlotIdx
end

-------------------------------------------------------------------------------
function UI_ZhuanPan:GetConstPhaseSpeed(Angle,Time)
	local _Speed

	_Speed = Angle / Time
	return _Speed
end

-------------------------------------------------------------------------------
function UI_ZhuanPan:GetConstPhaseAngle(Num)
	local _Angle

	_Angle = Num * 360.0
	return _Angle
end

-------------------------------------------------------------------------------
function UI_ZhuanPan:GetDecayPhaseTime(Speed,Angle)
	local _Time

	_Time = 2 * Angle / Speed
	return _Time
end
-------------------------------------------------------------------------------
function UI_ZhuanPan:GetAccer( StartV, time)
	local _Accer

	_Accer = -StartV / time
	return _Accer
end
-------------------------------------------------------------------------------
function UI_ZhuanPan:PlayIncreasePhase()
	local _ConstAnglePointer
	m_eNowPhasePointer = RoundPhase.PhaseType_SpeedUp
	m_fVPointer             = 0  --加速阶段初始速度
	_ConstAnglePointer      = self:GetConstPhaseAngle(m_iConstNumPointer) --匀速阶段总角度，用来算匀速阶段速度
	local _IncreaseAngle = self:GetConstPhaseAngle(m_iIncreaseNumPointer) --加速阶段总角度
	local _DestVPointer = self:GetConstPhaseSpeed(_ConstAnglePointer, m_fConstTime)  --加速到目标速度，也就是匀速阶段的速度
	m_fIncTimePointer     = self:GetDecayPhaseTime(_DestVPointer, _IncreaseAngle)
	m_fAccerPointer         = self:GetAccer(-_DestVPointer, m_fIncTimePointer)
	self:StartPlay()
	self:PlayAnimation(self:GetAnimations("chilun"),0,1)
end

--转盘匀速旋转
-------------------------------------------------------------------------------
function UI_ZhuanPan:PlayConstPhase()
	local _ConstAnglePointer

	m_eNowPhasePointer = RoundPhase.PhaseType_Const
                                     
	_ConstAnglePointer      = self:GetConstPhaseAngle(m_iConstNumPointer)
	m_fVPointer             = self:GetConstPhaseSpeed(_ConstAnglePointer, m_fConstTime)
	
	m_fAccerPointer         = 0


end
-------------------------------------------------------------------------------
function UI_ZhuanPan:GetNewSlotIdx(OrignSlotIdx,SlotOffset)
	return (OrignSlotIdx + SlotOffset) % m_iSlotNum
end

--指针减速旋转
-------------------------------------------------------------------------------
function UI_ZhuanPan:PlayPointerDecayPhase()
	local _WonRewardFinalSlotIdx
	local _WonRewardSlotOffset
	local _Reward0OrignSlotIdx
	local _Reward0FinalSlotIdx

	m_eNowPhasePointer = RoundPhase.PhaseType_Decay
	m_fTimeEnterDecayPointer = m_fNowTime

	_Reward0OrignSlotIdx    = m_iReward0SlotIdx

	_Reward0FinalSlotIdx    = self:GetNewSlotIdx(_Reward0OrignSlotIdx, m_iSlotOffsetTurnTable)

	_WonRewardFinalSlotIdx  = self:GetWonRewardSlotIdx(_Reward0FinalSlotIdx, m_iWonRewardIdx)

	_WonRewardSlotOffset    = self:GetSlotOffSet(m_iLastWonRewardSlotIdx, _WonRewardFinalSlotIdx)

	m_fDecayAnglePointer    = self:GetNeedRotateAngle(m_iDecayNumPointer, _WonRewardSlotOffset)

	m_fDecayTimePointer     = self:GetDecayPhaseTime(m_fVPointer, m_fDecayAnglePointer)

	m_fAccerPointer         = self:GetAccer(m_fVPointer, m_fDecayTimePointer)

	m_iLastWonRewardSlotIdx = _WonRewardFinalSlotIdx
end
-------------------------------------------------------------------------------
function UI_ZhuanPan:NormalizeOffSetAngle(LastLogicAngle)
	local _Result

	_Result = LastLogicAngle
	while _Result > 360 do
		_Result = _Result - 360
	end
	return _Result
end
-------------------------------------------------------------------------------
function UI_ZhuanPan:GetNowLogicAngle( StartSpeed,  Accer,  time)
	local _Angle

	_Angle = StartSpeed * time + 0.5 * Accer * time * time
	return _Angle
end

-------------------------------------------------------------------------------
function UI_ZhuanPan:StartPlay()
	m_fTimeEnterDecayPointer = 0
	m_bIsPlaying = true
end
-------------------------------------------------------------------------------
function UI_ZhuanPan:PlayPointer(delta)
	if m_fNowTime > m_fIncTimePointer then
		if m_eNowPhasePointer == RoundPhase.PhaseType_SpeedUp then
			self:PlayConstPhase()
			return
		end
	end
	if m_fNowTime > m_fConstTime + m_fIncTimePointer then
		if m_eNowPhasePointer == RoundPhase.PhaseType_Const then
			self:PlayPointerDecayPhase()
			return
		end
	end
	if m_eNowPhasePointer == RoundPhase.PhaseType_Decay and m_fNowTime > m_fIncTimePointer + m_fConstTime + m_fDecayTimePointer then
		m_bIsPlaying = false
		m_fLastOffSetPointer = m_fLastOffSetPointer + m_fDecayAnglePointer
		m_fLastOffSetPointer = self:NormalizeOffSetAngle(m_fLastOffSetPointer)

		self:SetRotation( m_fLastOffSetPointer)	
		self:PopReward()
		return
	end
	m_fLogicAnglePointer = m_fLastOffSetPointer + self:GetNowLogicAngle(m_fVPointer, m_fAccerPointer, m_fNowTime - m_fTimeEnterDecayPointer)
	m_fLogicAnglePointer = self:NormalizeOffSetAngle(m_fLogicAnglePointer)
	-- a_("m_fLogicAnglePointer "..m_fLogicAnglePointer)
	self:SetRotation( m_fLogicAnglePointer)
end
-------------------------------------------------------------------------------
function UI_ZhuanPan:SetRotation(_angle)
	self.u_CanvasPanelPan:SetRenderAngle(_angle)
end

-------------------------------------------------------------------------------
function UI_ZhuanPan:CustomTick(delta)
	if m_bIsPlaying then
		m_fNowTime = m_fNowTime + delta
		self:PlayPointer(delta)
	else
		m_fNowTime = 0
	end
	self:TickLeftTime()
end
-------------------------------------------------------------------------------
function UI_ZhuanPan:TickLeftTime()
	if self.m_FirstRewardInfo then
		if self.m_FirstRewardInfo.VipResetType == EnumConfig.VipResetType.Daily then
			local l_end = self.m_OAInfo.timeBegin+self.m_OAInfo.duration
			local l_time = os.difftime(l_end, DateToTime())
			local _FiveClockTime, _SecondsFromNow = GetNextClockTimeAndDiffNow(5)
			if l_time < _SecondsFromNow then
				_SecondsFromNow = l_time
			end
			local dateTable = ConvertSecondsToDate(_SecondsFromNow)
			local strLable = ""
			if dateTable.day > 0 then
				strLable = GetLocalizePrintf("L_LotteryTimeFormat2",dateTable.day*24+dateTable.hour,dateTable.min,dateTable.sec)
			else
				strLable = GetLocalizePrintf("L_LotteryTimeFormat2",dateTable.hour,dateTable.min,dateTable.sec)
			end
			self.u_TextBlockLeftTime:SetText(strLable)
		elseif self.m_FirstRewardInfo.VipResetType == EnumConfig.VipResetType.Weekly then
			--周一凌晨5点

		elseif self.m_FirstRewardInfo.VipResetType == EnumConfig.VipResetType.Monthly then
			--1号凌晨5点
		else --不填就是活动结束倒计时
			local l_end = self.m_OAInfo.timeBegin+self.m_OAInfo.duration
			local l_time = os.difftime(l_end, DateToTime())
			local dateTable = ConvertSecondsToDate(l_time)
			local strLable = ""
			if dateTable.day > 0 then
				strLable = GetLocalizePrintf("L_LotteryTimeFormat2",dateTable.day*24+dateTable.hour,dateTable.min,dateTable.sec)
			else
				strLable = GetLocalizePrintf("L_LotteryTimeFormat2",dateTable.hour,dateTable.min,dateTable.sec)
			end
			self.u_TextBlockLeftTime:SetText(strLable)
		end
	end
end

function UI_ZhuanPan:InitUI(_OAInfo)
	self.m_OAInfo = _OAInfo
	self:CreateRewardList()
	self:CalcPlayTime()
end
-------------------------------------------------------------------------------
function UI_ZhuanPan:CreateRewardList()
	if self.m_ShowList and #self.m_ShowList > 0 then return end
	self.m_ShowList = {}
	local l_rewardnum = 1
	local l_idx = 1
	for i,v in ipairs(self.m_OAInfo.rewardList) do
		if v.CompareVar == 0 and v.GetNum == 0 then
			for m,n in ipairs(v.objList) do
				if l_rewardnum == m_UnkhownIndex then
					local l_specailitem = require "ui.UI_ItemFlash":NewChildWidgetEx(self,self.pc,self.pc)
					l_specailitem:SetWidgetSize(self:Wnd("ScaleBox_Icon"..l_rewardnum).Slot:GetSize().X)
					l_specailitem:InitUI(EnumConfig.RewardType.Prop,m_SpecialItemID,nil,true)
					self:Wnd("ScaleBox_Icon6"):SetContent(l_specailitem)
					local l_info = {}
					l_info.type = 0
					l_info.id = 0
					l_info.value = 0
					table.insert(self.m_ShowList,l_info)
					l_rewardnum = l_rewardnum + 1
				end
				local l_item = require "ui.UI_ItemFlash":NewChildWidgetEx(self,self.pc,self.pc)
				if l_item then
					l_item:SetWidgetSize(self:Wnd("ScaleBox_Icon"..l_rewardnum).Slot:GetSize().X)
					local l_ret = l_item:InitUI(n.type,n.id,math.abs(n.value),true)
					if l_ret then
						local l_quality = l_item:GetQuality()
						if n.type == EnumConfig.RewardType.Pet or (n.type == EnumConfig.RewardType.Coin and n.id == EnumConfig.EnumCoinType.RMB)
						or (l_quality and l_quality > 4) then
							l_item:ShowEffectUI(true)
						end
						table.insert(self.m_ShowList,n)
						self:Wnd("ScaleBox_Icon"..l_rewardnum):SetContent(l_item)
					end
				end
				if l_idx == 1 then --特殊奖励，带特效
					l_idx = 0
				end
				l_rewardnum = l_rewardnum + 1
			end
		end
	end
end

-------------------------------------------------------------------------------
function UI_ZhuanPan:CalcPlayTime()
	self.m_TimeList = {}  --可抽奖活动次数列表
	self.m_TimeCount = 0
	self.m_TxtList = self.m_TxtList or {}
	local l_txtnum = #self.m_TxtList
	local l_idx = 1
	local l_scrollwidget = nil
	for i,v in ipairs(self.m_OAInfo.rewardList) do
		if v.GetNum > 0 then  --抽奖组
			if not self.m_FirstRewardInfo then self.m_FirstRewardInfo = v end
			local l_candraw = GlobalPlayerData.Data_OA:CanActivityDraw(self.m_OAInfo.oaId,i)
			local l_drawcount,l_progress = GlobalPlayerData.Data_OA:GetOADrawCountAndProgress(self.m_OAInfo.oaId,i)
			if l_candraw then
				self.m_TimeList[i] = v.GetNum - l_drawcount
				self.m_TimeCount = self.m_TimeCount + self.m_TimeList[i]
			end
			if v.CompareVar > 0 then --非免费次数
				local l_matchgoal = l_progress >= v.CompareVar
				if l_txtnum == 0 then
					local l_txtui = require "ui.oa.UI_ZhuanPanTxt":NewChildWidgetEx(self,self.pc,self.pc)
					l_txtui:InitUI(v.CompareVar,v.GetNum,l_matchgoal)
					self.u_WrapBoxTips:AddChildToWrapBox(l_txtui)
					table.insert(self.m_TxtList,l_txtui)
					if not l_candraw and not l_scrollwidget then l_scrollwidget = l_txtui end
				elseif self.m_TxtList[l_idx] then
					self.m_TxtList[l_idx]:InitUI(v.CompareVar,v.GetNum,l_matchgoal)
					if not l_candraw and not l_scrollwidget then l_scrollwidget = self.m_TxtList[l_idx] end
				end
				l_idx = l_idx + 1
			end
		end
	end

	self:SetTimer(0.03, 1, InsCallBack(self.MakeScrollToEnd,self,l_scrollwidget))

	self.u_TextBlockTime:SetText(self.m_TimeCount)
end
-------------------------------------------------------------------------------
function UI_ZhuanPan:MakeScrollToEnd(l_scrollwidget)
	if l_scrollwidget then
		self.u_ScrollBoxTips:ScrollWidgetIntoView(l_scrollwidget,true)
	end
end

-------------------------------------------------------------------------------
function UI_ZhuanPan:ShowItemTipsEx(_Widget)
	local l_posWidget = self:Wnd("Border_TipPos")
	local _TipSize = _Widget.UIfather.ItemTipFrame.UI_IMG_TipsBg.Slot:GetSize()
	local _Position = self:GetTipUIPos(l_posWidget, _TipSize.X, _TipSize.Y)
	_Widget.UIfather.ItemTipFrame:UpdatePanelPos(_Position)
	_Widget.CloseEff = true
end
-------------------------------------------------------------------------------
function UI_ZhuanPan:ShowPetWidgetTipEx(_Widget)
	local l_posWidget = self:Wnd("Border_TipPos")
	local _TipSize = _Widget.UIfather.PetTipFrame.UI_IMG_TipsBg.Slot:GetSize()
	local _Position = self:GetTipUIPos(l_posWidget, _TipSize.X, _TipSize.Y)
	_Widget.UIfather.PetTipFrame:SetTipPosition(_Position)
	_Widget.CloseEff = true
end
-------------------------------------------------------------------------------
function UI_ZhuanPan:ShowFragmentTipsEx(_Widget)
	local l_posWidget = self:Wnd("Border_TipPos")
	local _TipSize = _Widget.UIfather.FragmentTipFrame.UI_IMG_TipsBg.Slot:GetSize()
	local _Position = self:GetTipUIPos(l_posWidget, _TipSize.X, _TipSize.Y)
	_Widget.UIfather.FragmentTipFrame:UpdatePanelPos(_Position)
	_Widget.CloseEff = true
end
-------------------------------------------------------------------------------
function UI_ZhuanPan:ShowEquipTipsEx(_Widget)
	local l_posWidget = self:Wnd("Border_TipPos")
	local _TipSize = _Widget.UIfather.EquipTipFrame.CanvasPanel_BG.Slot:GetSize()
	local _Position = self:GetTipUIPos(l_posWidget, _TipSize.X, _TipSize.Y)
	_Widget.UIfather.EquipTipFrame.CanvasPanel_BG.Slot:SetPosition(_Position)
	_Widget.CloseEff = true
end
-------------------------------------------------------------------------------
function UI_ZhuanPan:ShowMedalTipsEx(_Widget)
	local l_posWidget = self:Wnd("Border_TipPos")
	local _TipSize = _Widget.UIfather.MedalTipFrame.CanvasPanel_BG.Slot:GetSize()
	local _Position = self:GetTipUIPos(l_posWidget, _TipSize.X, _TipSize.Y)
	_Widget.UIfather.MedalTipFrame.CanvasPanel_BG.Slot:SetPosition(_Position)
	_Widget.CloseEff = true
end
-------------------------------------------------------------------------------
function UI_ZhuanPan:ShowHeldItemTipsEx(_Widget)
	local l_posWidget = self:Wnd("Border_TipPos")
	local _TipSize = _Widget.UIfather.HeldItemTipFrame.CanvasPanel_BG.Slot:GetSize()
	local _Position = self:GetTipUIPos(l_posWidget, _TipSize.X, _TipSize.Y)
	_Widget.UIfather.HeldItemTipFrame.CanvasPanel_BG.Slot:SetPosition(_Position)
	_Widget.CloseEff = true
end
function UI_ZhuanPan:OnClickStart()
	if not GlobalPlayerData.Data_OA:IsActivityOpened(self.m_OAInfo.oaId) then
		self.pc:PopMsg(GetLocalize("L_OAEnd"))
		return
	end
	if m_bIsPlaying or m_drawing then
		return
	end
	if self.m_TimeCount == 0 then
		local _Content = GetLocalize("L_ZhuanPanTips1")
		local _CallBack = InsCallBack(self.OnClickAddTime, self)
		self.pc:OpenCommonTipTwoBtn(_Content,_CallBack)
		return
	end
	self:GetReward()
	-- m_iWonRewardIdx = math.random(0,9)
	-- a_("m_iWonRewardIdx "..m_iWonRewardIdx)
	-- self:PlayIncreasePhase()
	
end
-------------------------------------------------------------------------------
function UI_ZhuanPan:GetReward()
	self.m_drawing = true
	for k,v in pairsByKeys(self.m_TimeList) do
		self.m_rewardIndex = k
		break
	end
	
	_sendnetmsg("OAReward",self.m_OAInfo.oaId,self.m_rewardIndex,{})
	return true
end
-------------------------------------------------------------------------------
function UI_ZhuanPan:GetRewardIndex(reward)
	local l_reward = reward
	local l_ret = m_UnkhownIndex - 1  --索引是0-max
	if l_reward then
		for i,v in ipairs(self.m_ShowList) do
			if v.type == l_reward.type and v.id == l_reward.id and v.value == l_reward.value then
				l_ret = i - 1
				break
			end
		end
	end
	return l_ret
end

-------------------------------------------------------------------------------
function UI_ZhuanPan:OnOARewardResult(response)
	if response.oaId ~= self.m_OAInfo.oaId or response.rewardIndex ~= self.m_rewardIndex then return end
	self.m_drawing = false
	if response.error_code == 0 then
		m_iWonRewardIdx = self:GetRewardIndex(self.m_Reward)
		a_("m_iWonRewardIdx "..m_iWonRewardIdx)
		self:PlayIncreasePhase()
	else
		self.pc:PopMsg(GetErrorCodeTip(response.error_code))
	end
end
-------------------------------------------------------------------------------
function UI_ZhuanPan:PopReward()
	self:StopAnimation(self:GetAnimations("chilun"))
	--弹获得新宠物界面
	if EnumConfig.RewardType.Pet == self.m_Reward.type then
		local _Callback = InsCallBack(self.OnCloseGetPetFrame, self)
		self.pc:OpenGeneralGetPetFrame(self.m_Reward.id,nil,nil,_Callback)
		self.pc.OAFrame:SetVisibility(ESlateVisibility.Collapsed)
		self:SetVisibility(ESlateVisibility.Collapsed)
	else
		self.pc:PopGeneralGetReward({self.m_Reward})
	end

end
-------------------------------------------------------------------------------
function UI_ZhuanPan:OnCloseGetPetFrame()
	self.pc.OAFrame:SetVisibility(ESlateVisibility.SelfHitTestInvisible)
	self:SetVisibility(ESlateVisibility.SelfHitTestInvisible)
end

-------------------------------------------------------------------------------
function UI_ZhuanPan:OnObjChangeNotify(response)
	self.m_Reward = response.list[1]
end

-------------------------------------------------------------------------------
function UI_ZhuanPan:OnOARefresh()
	self:CalcPlayTime()
end

-------------------------------------------------------------------------------
function UI_ZhuanPan:OnClickAddTime()
	self.pc:OpenRechargeMainFrame(self.classname)
	return true
end

function UI_ZhuanPan:InitEvent()
	self.u_ButtonStart:Event("OnClicked", self.OnClickStart, self)
	self.u_ButtonAddTime:Event("OnClicked", self.OnClickAddTime, self)
	_recvnetmsg("OARewardResult",InsCallBack(self.OnOARewardResult,self))
	_recvnetmsg("OARefresh",InsCallBack(self.OnOARefresh,self))
	_recvnetmsg("ObjChangeNotify", InsCallBack(self.OnObjChangeNotify, self)) 
end
function UI_ZhuanPan:BindWidgets()
	self.u_CanvasPanelPan = self:Wnd("CanvasPanel_Pan")
	self.u_ScaleBoxIcon1 = self:Wnd("ScaleBox_Icon1")
	self.u_ScaleBoxIcon2 = self:Wnd("ScaleBox_Icon2")
	self.u_ScaleBoxIcon3 = self:Wnd("ScaleBox_Icon3")
	self.u_ScaleBoxIcon4 = self:Wnd("ScaleBox_Icon4")
	self.u_ScaleBoxIcon5 = self:Wnd("ScaleBox_Icon5")
	self.u_ScaleBoxIcon7 = self:Wnd("ScaleBox_Icon7")
	self.u_ScaleBoxIcon8 = self:Wnd("ScaleBox_Icon8")
	self.u_ScaleBoxIcon9 = self:Wnd("ScaleBox_Icon9")
	self.u_ScaleBoxIcon10 = self:Wnd("ScaleBox_Icon10")
	self.u_ScaleBoxIcon6 = self:Wnd("ScaleBox_Icon6")
	self.u_ImagePointer = self:Wnd("Image_Pointer")
	self.u_TextBlockLeftTime = self:Wnd("TextBlock_LeftTime")
	self.u_ButtonAddTime = self:Wnd("Button_AddTime")
	self.u_ButtonStart = self:Wnd("Button_Start")
	self.u_TextBlockTime = self:Wnd("TextBlock_Time")
	self.u_ScrollBoxTips = self:Wnd("ScrollBox_Tips")
	self.u_WrapBoxTips = self:Wnd("WrapBox_Tips")
end






return UI_ZhuanPan
