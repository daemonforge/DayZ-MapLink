class CfgPatches
{
	class MapLink
	{
		requiredVersion=0.1;
		requiredAddons[]={
			"UniversalApi"
		};
	};
};

class CfgMods
{
	class MapLink
	{
		dir="MapLink";
        name="MapLink";
        credits="DaemonForge, Iceblade, Dumpgrah";
        author="DaemonForge";
        authorID="0";
        version="0.1";
        extra=0;
        type="mod";
		inputs = "MapLink/inputs/inputs.xml";
	    dependencies[]={ "Game", "World", "Mission"};
	    class defs
	    {
			
			class imageSets
			{
				files[]=
				{
					"MapLink/gui/maplink_icons.imageset",
					"MapLink/gui/maplink_money.imageset"
				};
			};
			class gameScriptModule
            {
				value = "";
                files[]={
					"MapLink/scripts/3_Game",
					"MapLink/scripts/Common"
				};
            };
			class worldScriptModule
            {
                value="";
                files[]={ 
					"MapLink/scripts/4_World" ,
					"MapLink/scripts/Common"
				};
            };
			
	        class missionScriptModule
            {
                value="";
                files[]={
					"MapLink/scripts/5_Mission" ,
					"MapLink/scripts/Common"
				};
            };
        };
    };
};