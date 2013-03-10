function recordtest()
	a = fill_surface(32, 32, 255, 0, 0);
	b = fill_surface(32, 32, 0, 255, 0);
	c = fill_surface(32, 32, 0,   0, 255);

	show_image(a);
	show_image(b);
	show_image(c);

	image_transform_cycle(a, 1);
	image_transform_cycle(b, 1);
	image_transform_cycle(c, 1);

	move_image(a, 0, 0);
	move_image(b, VRESW - 32, 0);
	move_image(c, 0, VRESH - 32);

	move_image(a, VRESW - 32, 0, 20);
	move_image(b, 0, VRESH - 32, 20);
	move_image(c, 0, 0, 20);

	move_image(a, 0, VRESH - 32, 20);
	move_image(b, 0, 0, 20);
	move_image(c, VRESW - 32, 0, 20);

	move_image(a, 0, 0, 20);
	move_image(b, VRESW - 32, 0, 20);
	move_image(c, 0, VRESH - 32, 20);

	game = list_games({target = "scummvm"});

	if (game == nil) then
		error("game not found, giving up.");
		shutdown();
	end

	recording = false

	vid = launch_target(game[2].gameid, LAUNCH_INTERNAL, function(source, stat)
	if (stat.kind == "resized") then
		resize_image(source, VRESW, VRESH);
		show_image(source);
		
		if (not recording) then
			recording = true
			dstvid = fill_surface(VRESW, VRESH, 0, 0, 0, 320, 240);
			resize_image(source, VRESW, VRESH);
			show_image(dstvid);
			define_recordtarget(dstvid, "testout.mkv", "acodec=VORBIS:vcodec=FFV1:fps=50:vpreset=8", {source, a, b, c}, {stat.source_audio}, RENDERTARGET_DETACH, RENDERTARGET_SCALE, -1);
		end
	end
end)

end

function recordtest_input(iotbl)
	target_input(vid, iotbl);
end

