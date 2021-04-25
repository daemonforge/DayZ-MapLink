modded class PlayerBase extends ManBase{
	
	protected string m_TransferPoint = "";
	
	bool UApiSaveTransferPoint(string point = ""){
		m_TransferPoint = point;
		return true;
	}
	
	int GetQuickBarEntityIndex(EntityAI entity){
		return m_QuickBarBase.FindEntityIndex(entity);
	}
	

    override void OnStoreSave(ParamsWriteContext ctx)
    {
        super.OnStoreSave(ctx);
		//Making sure not to save freshspawns or dead people, dead people logic is handled in EEKilled
		if (!GetGame().IsClient() && GetHealth("","Health") > 0 && StatGet(AnalyticsManagerServer.STAT_PLAYTIME) > 90 ){ 
			this.SavePlayerToUApi();
		}
    }
	
	void SavePlayerToUApi(){
		if (this.GetIdentity() && GetGame().IsServer()){
			autoptr PlayerDataStore teststore = new PlayerDataStore(PlayerBase.Cast(this));
			UApi().db(PLAYER_DB).Save("TheHive", this.GetIdentity().GetId(), teststore.ToJson());
			delete teststore;
			//NotificationSystem.SimpleNoticiation(" You're Data has been saved to the API", "Notification","Notifications/gui/data/notifications.edds", -16843010, 10, this.GetIdentity());
		}
	}
	
	void OnUApiSave(ref PlayerDataStore data){
		int i = 0;
		data.m_TimeSurvivedValue = StatGet(AnalyticsManagerServer.STAT_PLAYTIME);
		data.m_PlayersKilledValue = StatGet(AnalyticsManagerServer.STAT_PLAYERS_KILLED);
		data.m_InfectedKilledValue = StatGet(AnalyticsManagerServer.STAT_INFECTED_KILLED);
		data.m_DistanceTraveledValue = StatGet(AnalyticsManagerServer.STAT_DISTANCE);
		data.m_LongRangeShotValue = StatGet(AnalyticsManagerServer.STAT_LONGEST_SURVIVOR_HIT );
		data.m_LifeSpanState = GetLifeSpanState();
		data.m_LastShavedSeconds = GetLastShavedSeconds();
		data.m_BloodType = GetStatBloodType().Get();
		data.m_HasBloodTypeVisible = HasBloodyHands();
		data.m_HasBloodyHandsVisible = HasBloodTypeVisible();
		/*for(i = 0; i < GetPlayerStats().GetPCO(GetGame().SaveVersion()).Get().Count(); i++){
			data.AddPlayerStat(GetPlayerStats().GetPCO(GetGame().SaveVersion()).Get().Get(i));
			Print("[UAPI] Saving Stat ["+ GetPlayerStats().GetPCO(GetGame().SaveVersion()).Get().Get(i).GetType() + "] Label: " + GetPlayerStats().GetPCO(GetGame().SaveVersion()).Get().Get(i).GetLabel() + " Value: " + GetPlayerStats().GetPCO(GetGame().SaveVersion()).Get().Get(i).Get());
		}*/
		for(i = 0; i < m_ModifiersManager.m_ModifierList.Count(); i++){
            ModifierBase mdfr = ModifierBase.Cast(m_ModifiersManager.m_ModifierList.GetElement(i));
            if (mdfr && mdfr.IsActive() && mdfr.IsPersistent()) { 
				data.AddModifier( mdfr.GetModifierID(), mdfr.GetAttachedTime());
			}
		}
		for(i = 0; i < m_AgentPool.m_VirusPool.Count();i++){
			data.AddAgent(m_AgentPool.m_VirusPool.GetKey(i), m_AgentPool.m_VirusPool.GetElement(i));
		}
		data.m_TransferPoint = m_TransferPoint;
		data.m_BrokenLegState = m_BrokenLegState;
		
		data.m_BleedingBits = GetBleedingBits();
		if (GetBleedingManagerServer()){
			GetBleedingManagerServer().OnUApiSave(data);
		} else {
			Print("[UAPI] Bleeding Manager is NULL");
		}
		
	}
	
	void OnUApiLoad(ref PlayerDataStore data){
		int i = 0;
		
		StatUpdate(AnalyticsManagerServer.STAT_PLAYTIME, data.m_TimeSurvivedValue );
		StatUpdate(AnalyticsManagerServer.STAT_PLAYERS_KILLED, data.m_PlayersKilledValue);
		StatUpdate(AnalyticsManagerServer.STAT_INFECTED_KILLED, data.m_InfectedKilledValue);
		StatUpdate(AnalyticsManagerServer.STAT_DISTANCE, data.m_DistanceTraveledValue);
		StatUpdate(AnalyticsManagerServer.STAT_LONGEST_SURVIVOR_HIT, data.m_LongRangeShotValue );
		SetLifeSpanStateVisible(data.m_LifeSpanState);
		SetLastShavedSeconds(data.m_LastShavedSeconds);
		SetBloodyHands(data.m_HasBloodyHandsVisible);
		for (i = 0; i < data.m_Modifiers.Count(); i++){
			if (data.m_Modifiers.Get(i)){
				ModifierBase mdfr = m_ModifiersManager.GetModifier(data.m_Modifiers.Get(i).ID());
				if (mdfr.IsTrackAttachedTime() && data.m_Modifiers.Get(i).Value() >= 0){
					mdfr.SetAttachedTime(data.m_Modifiers.Get(i).Value());
				}
				m_ModifiersManager.ActivateModifier(data.m_Modifiers.Get(i).ID(), EActivationType.TRIGGER_EVENT_ON_CONNECT);
			}
		}
		for(i = 0; i < data.m_Agents.Count();i++){
			m_AgentPool.SetAgentCount(data.m_Agents.Get(i).ID(), data.m_Agents.Get(i).Value());
		}
		/*if (data.m_PlayerStats.Count() == GetPlayerStats().GetPCO(GetGame().SaveVersion()).Get().Count()){
			for(i = 0; i < GetPlayerStats().GetPCO(GetGame().SaveVersion()).Get().Count(); i++){
				GetPlayerStats().GetPCO(GetGame().SaveVersion()).Get().Get(i).SetByFloat(data.m_PlayerStats.Get(i));
				Print("[UAPI] Saved Stat ["+ GetPlayerStats().GetPCO(GetGame().SaveVersion()).Get().Get(i).GetType() + "] Label: " + GetPlayerStats().GetPCO(GetGame().SaveVersion()).Get().Get(i).GetLabel() + " Value: " + GetPlayerStats().GetPCO(GetGame().SaveVersion()).Get().Get(i).Get());
			}
		} else {
			Print("[UAPI] [ERROR] Stats didn't match?");
		}*/
		data.m_TransferPoint = "";
		m_TransferPoint = "";
		m_BrokenLegState = data.m_BrokenLegState;
		//SetBleedingBits(data.m_BleedingBits);
		if (GetBleedingManagerServer()){	
			GetBleedingManagerServer().OnUApiLoad(data);
		} else {
			Print("[UAPI] Bleeding Manager is NULL");
		}
		GetStatWet().Set(data.m_Stat_Wet);
		GetStatSpecialty().Set(data.m_Stat_Specialty);
		GetStatHeatBuffer().Set(data.m_Stat_HeatBuffer);
		GetStatStamina().Set(data.m_Stat_Stamina);
		GetStatToxicity().Set(data.m_Stat_Toxicity);
		GetStatWater().Set(data.m_Stat_Water);
		GetStatEnergy().Set(data.m_Stat_Energy);
		GetStatBloodType().Set(data.m_BloodType);
		
		SetBloodType(data.m_BloodType);
		SetBloodTypeVisible(data.m_HasBloodTypeVisible);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(this.SetSynchDirty);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.SendUApiAfterLoadClient, 300);
	}
	
	void SendUApiAfterLoadClient(){
		RPCSingleParam(155494151, new Param1<bool>( true ), true, GetIdentity());
	}
	
	override void OnDisconnect(){
		//If the player has played less than 1 1/2 minutes just kill them so their data doesn't save to the local database
		if ( StatGet(AnalyticsManagerServer.STAT_PLAYTIME) <= 90){ 
			SetHealth("","", 0); 
		}
		super.OnDisconnect();
	}
	
	
	override void EEKilled( Object killer )
	{
		//Only save dead people who've been on the server for more than 1 1/2 minutes and who arn't tranfering
		if (m_TransferPoint == "" && StatGet(AnalyticsManagerServer.STAT_PLAYTIME) > 90){
			this.SavePlayerToUApi();
		}
		//If they are transfering delete or a fresh spawn just delete the body
		if (m_TransferPoint != "" || StatGet(AnalyticsManagerServer.STAT_PLAYTIME) <= 90){
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.Delete, 300,false);
		}
		
		super.EEKilled( killer );
	}

	
	void UApiKillAndDeletePlayer(){
		SetHealth("","", 0);
	}
	
	
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		Param1<bool> data;
		
		if (rpc_type == 155494151 && GetGame().IsClient()) {
			if (ctx.Read(data))	{
				if (data.param1){
					UApiAfterLoadClient();
				}
			}
		}
	}
	
	void UApiAfterLoadClient(){
		this.UpdateInventoryMenu();
	}
	
}