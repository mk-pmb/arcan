local function datashare(wnd)
	local res = awbwman_setup_cursortag(sysicons.floppy);
	res.kind = "media";
	res.source = wnd;
	return res;
end

local function inputlay_sel(icn, wnd)
	if (awbwman_ispopup(icn.vid)) then
		wnd:focus();
		return nil;
	end

	wnd:focus();
	local lst = inputed_configlist();

	table.insert(lst, 1, "Disable");
	local str = table.concat(lst, [[\n\r]]);
	local vid, lines = desktoplbl(str);

	awbwman_popup(vid, lines, function(ind)
		wnd.inp_cfg = inputed_getcfg(lst[ind]); 
	end, {ref = icn.vid});
end

--
-- Re-uses the same patterns as gridle
-- If the user wants to load savestates with other names, 
-- he'll have to drag'n'drop from the desktop group
--
function awbtarget_listsnaps(tgtwin, gametbl)
	local base = glob_resource(string.format("savestates/%s_%s_*", 
		gametbl.target, 
		gametbl.setname and gametbl.setname or ""), SHARED_RESOURCE);
	
	if (base and #base > 0) then
-- The number of possible savestates make this one difficult 
-- to maintain as a popup, so use a list window
		local newwnd = awbwman_listwnd(
			menulbl(gametbl.title .. ":savestates"), deffont_sz, linespace, {1.0},
			function(filter, ofs, lim, iconw, iconh)
				local res = {};
				local ul  = ofs + lim;
				for i=ofs, ul do
					table.insert(res, {
						name = base[i],
						trigger = function() restore_target(tgtwin.recv, base[i]); end, 
						name = base[i],
						cols = {base[i]}});
				end
				return res, #base;
			end, desktoplbl);

-- Make sure that this window disappears when / if the parent 
-- dies / is forcibly closed
		table.insert(tgtwin.cascade, newwnd);
		local olddes = newwnd.on_destroy;
		newwnd.on_destroy = function(self)
			if (olddes) then
				olddes(self);
			end
			for i=#tgtwin.cascade,1,-1 do
				if (tgtwin.cascade[i] == newwnd) then
					table.remove(tgtwin.cascade, i);
					break;
				end
			end
		end

	end
end
	
--target_postfilter_args(internal_vid, 1, settings.ntsc_hue, settings.ntsc_saturation, settings.ntsc_contrast);
--target_postfilter_args(internal_vid, 2, settings.ntsc_brightness, settings.ntsc_gamma, settings.ntsc_sharpness);
--target_postfilter_args(internal_vid, 3, settings.ntsc_resolution, settings.ntsc_artifacts, settings.ntsc_bleed);
--target_postfilter_args(internal_vid, 4, settings.ntsc_fringing);
--target_postfilter(internal_vid, settings.internal_toggles.ntsc and POSTFILTER_NTSC or POSTFILTER_OFF);

--
-- Target window
-- Builds upon a spawned window (pwin) and returns a 
-- suitable callback for use in launch_target- style commands.
-- Assumes a bar already existing in the "tt" spot to add icons to.
--
function awbwnd_target(pwin)
	local cfg = awbwman_cfg();
	local bartt = pwin.dir.tt;
	pwin.cascade = {}; 

	pwin.mediavol = 1.0;

	pwin.set_mvol = function(self, val)
		pwin.mediavol = val;
		local tmpvol = awbwman_cfg().global_vol * pwin.mediavol; 
		tmpvol = tmpvol < 0 and 0 or tmpvol;
		if (pwin.reca ~= nil) then
			audio_gain(pwin.reca, tmpvol);	
		end
	end
	
	bartt:add_icon("clone", "r", cfg.bordericns["clone"], 
		function() datashare(pwin); 
	end);

	bartt:add_icon("volume", "r", cfg.bordericns["volume"], function(self)
		if (not awbwman_ispopup(self)) then
			pwin:focus();
			awbwman_popupslider(0.01, pwin.mediavol, 1.0, function(val)
				pwin:set_mvol(val);
			end, {ref = self.vid});
		end
	end);

--
-- Popup save menu
--
	bartt:add_icon("save", "l", cfg.bordericns["save"], function(self)
		local list = {"Quicksave", "New..."};
	end);

	bartt:add_icon("pause", "l", cfg.bordericns["pause"], function(self) 
		if (pwin.paused) then
			pwin.paused = nil;
			resume_target(pwin.canvas.vid);
			image_sharestorage(cfg.bordericns["pause"], self.vid);
		else
			pwin.paused = true;
			suspend_target(pwin.canvas.vid);
			image_sharestorage(cfg.bordericns["play"], self.vid);
		end
	end);

--
-- Popup "saves" list filtered by target / game
--
	bartt:add_icon("load", "l", cfg.bordericns["load"], function(self)
		awbtarget_listsnaps(pwin, pwin.gametbl);
	end);

--
-- Set frameskip mode, change icon to play
--
	bartt:add_icon("ffwd", "l", cfg.bordericns["fastforward"], function(self)
		pwin:focus();
	end);

--
-- Missing: popup frameskip mode
-- popup filter mode
--

	bartt:add_icon("ginput", "r", cfg.bordericns["ginput"],
		function(self) 
			if (awbwman_reqglobal(pwin)) then
				image_shader(self.vid, "awb_selected"); 
			else
				image_shader(self.vid, "DEFAULT"); 
			end
		end);

	bartt:add_icon("input", "r", cfg.bordericns["input"],
		function(self) inputlay_sel(self, pwin); end);

	pwin.input = function(self, iotbl)
		if (pwin.inp_cfg == nil) then
			return;
		end
	
		local restbl = inputed_translate(iotbl, pwin.inp_cfg);
		if (restbl) then 
			for i,v in ipairs(restbl) do
				target_input(pwin.canvas.vid, v);
			end
		else -- hope that the hijacked target can do something with it anyway
			target_input(pwin.canvas.vid, iotbl);
		end
	end

-- add_icon filter
-- add_icon fast-forward
-- add_icon save(slot)

	local callback = function(source, status)
		if (status.kind == "frameserver_terminated") then
			pwin:update_canvas( color_surface(1, 1, 0, 0, 0) );

		elseif (status.kind == "loading") then
			print("show loading info..");

		elseif (status.kind == "resized") then
			pwin:update_canvas(source);
			pwin:resize(pwin.w, pwin.h);
		end
	end

	bartt.click = function() pwin:focus(); end
	local canvash = {
					own = function(self, vid) return vid == pwin.canvas.vid; end,
					click = function() pwin:focus(); end
	}

	bartt.name = "target_ttbar";
	canvash.name = "target_canvas";

	mouse_addlistener(bartt, {"click"});
	mouse_addlistener(canvash, {"click"});
	table.insert(pwin.handlers, bartt);
	table.insert(pwin.handlers, canvash);

	pwin:update_canvas( fill_surface(pwin.w, pwin.h, 100, 100, 100) );
	return callback;
end
