require("tts")
local enabled = false
local readall = false
local playing = false
local queue = {}
local menualpha = 0

concommand.Add("samplemul", function(ply, cmd, args)
	SetSamplerateMultiplier(args[1])
end)

concommand.Add("convert_ffmpeg", function(ply, cmd, args)
	ConvertFFMPEG(args[1], args[2])
end)

concommand.Add("playsample", function(ply, cmd, args)
	PlaySample(args[1])
end)

concommand.Add("tts_disable", function(ply, cmd, args)
	enabled = false
	playing = false
	queue = {}

	SpeakSAPI("")

	chat.AddText(Color(128, 128, 128), "[", Color(255, 255, 0), "gm_tts", Color(128, 128, 128), "] ", Color(255, 255, 255), "TTS Disabled.")
end)

concommand.Add("tts_enable", function(ply, cmd, args)
	enabled = true
	playing = false
	queue = {}

	SpeakSAPI("")

	chat.AddText(Color(128, 128, 128), "[", Color(255, 255, 0), "gm_tts", Color(128, 128, 128), "] ", Color(255, 255, 255), "TTS Enabled.")
end)

concommand.Add("tts_readall", function(ply, cmd, args)
	readall = tobool(args[1])
end)

if not gm_tts_old_chataddtext then
	gm_tts_old_chataddtext = chat.AddText
end

function gm_tts_parse(txt)
	if enabled then
		local validQuery = false
		local query = {}

		if string.sub(txt, 1, 5) == "<sam>" then
			query.engine = "SAPI"
			query.voice = "Name=Microsoft Sam"
			query.text = string.sub(txt, 6, #txt)
			validQuery = true
		end

		if string.sub(txt, 1, 6) == "<mike>" then
			query.engine = "SAPI"
			query.voice = "Name=Microsoft Mike"
			query.text = string.sub(txt, 7, #txt)
			validQuery = true
		end

		if string.sub(txt, 1, 6) == "<mary>" then
			query.engine = "SAPI"
			query.voice = "Name=Microsoft Mary"
			query.text = string.sub(txt, 7, #txt)
			validQuery = true
		end

		if string.sub(txt, 1, 7) == "<david>" then
			query.engine = "SAPI"
			query.voice = "Name=Microsoft David Desktop"
			query.text = string.sub(txt, 8, #txt)
			validQuery = true
		end

		if string.sub(txt, 1, 6) == "<zira>" then
			query.engine = "SAPI"
			query.voice = "Name=Microsoft Zira Desktop"
			query.text = string.sub(txt, 7, #txt)
			validQuery = true
		end

		if string.sub(txt, 1, 8) == "<espeak>" then
			query.engine = "SAPI"
			query.voice = "Name=eSpeak-EN-US"
			query.text = string.sub(txt, 9, #txt)
			validQuery = true
		end

		/*if string.sub(txt, 1, 5) == "<jan>" then
			query.engine = "SAPI"
			query.voice = "Name=Speechtech Jan"
			query.text = string.sub(txt, 6, #txt)
			validQuery = true
		end*/

		if string.sub(txt, 1, 6) == "<paul>" then
			query.engine = "DEC"
			query.voice = ""
			query.text = string.sub(txt, 7, #txt)
			validQuery = true
		end

		if string.sub(txt, 1, 5) == "<SAM>" then
			query.engine = "SAM"
			query.voice = ""
			query.text = string.sub(txt, 6, #txt)
			validQuery = true
		end

		if validQuery then
			table.insert(queue, query)
		end
	end
end

chat.AddText = function(...)
	gm_tts_old_chataddtext(...)

	local args = {...}

	if not readall then
		--PrintTable(args)
		--gm_tts_parse(args[#args])

		for k, v in pairs(args) do
			if type(v) == "string" and #v > 5 then
				gm_tts_parse(string.sub(v, 3, #v))
			end
		end
	else
		local voices = 
		{
			"<sam>"
		}

		local fulltxt = ""

		for k, v in pairs(args) do
			if type(v) == "string" then
				fulltxt = fulltxt .. v
			end
		end

		gm_tts_parse(table.Random(voices) .. fulltxt)
	end
end

hook.Add("Tick", "tts_play", function(cmd)
	if enabled then
		if FinishedPlayingSample() then
			if not playing and #queue > 0 then
				local query = queue[1]

				if query.engine == "SAPI" then
					ChangeVoiceSAPI(query.voice)
					SpeakSAPI(query.text)
				elseif query.engine == "DEC" then
					SpeakDEC(query.text)
				elseif query.engine == "SAM" then
					SpeakSAM(query.text)
				end

				table.remove(queue, 1)

				playing = true
			else
				playing = false
			end
		end
	end

	TickVoice()
end)

hook.Add("HUDPaint", "tts_infoscreen", function()
	menualpha = math.floor((Lerp(5 * FrameTime(), menualpha, enabled and 255 or 0)))

	local rainbow = HSVToColor((CurTime() * 200) % 360, 1, 1)
	local rainbow2 = HSVToColor((100 + CurTime() * 200) % 360, 1, 1)

	rainbow.a = menualpha
	rainbow2.a = menualpha

	surface.SetDrawColor(0, 0, 0, menualpha)
	surface.DrawRect(42, 250, 310, 75)

	surface.SetDrawColor(rainbow)
	surface.DrawOutlinedRect(42, 250, 310, 75)

	local textus = string.format("Lmao power.\nStatus: %s\nRemaining: %i", FinishedPlayingSample() and "Idle" or "Busy", table.Count(queue))
	draw.DrawText(textus, "Trebuchet24", 50 + 2, 250 + 2, rainbow2)
	draw.DrawText(textus, "Trebuchet24", 50, 250, rainbow)
end)