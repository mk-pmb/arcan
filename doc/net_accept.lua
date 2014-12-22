-- net_accept
-- @short: Request that the networking frameserver accepts
-- a specific connection id.
-- @inargs: serverid, domain
-- @longdescr: The normal life-cycle for a networking
-- session is that the server listens for incoming connections.
-- When a client connects, an event is received on the
-- listening callback function and the server needs to explicitly
-- change the client state to active by calling net_accept
-- or terminate it with net_disconnect.
-- @group: network
-- @note: specifying 0 is an invalid domain and a terminal state transition.
-- @cfunction: net_accept
-- @related: net_authenticate, net_disconnect
function main()
#ifdef MAIN
#endif
#ifdef ERROR
#endif
end
