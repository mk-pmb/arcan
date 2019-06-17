-- restore_target
-- @short: Request that the target frameserver session
-- restores from a previously made snapshot.
-- @inargs: vid:target, string:resource
-- @inargs: vid:target, string:resource, number:namespace
-- @inargs: vid:target, string:resource, number:namespace, string:description
-- @outargs: bool:exist
-- @longdescr:
-- Open *resource* from the APPL_STATE_RESOURCE and send to *target*
-- requesting that it restores its state from the resource. It returns
-- true of the resource was found and sent successfully.
-- If another *namespace* is provided from the normal set of
-- read- resources, It will be opened read only and sent as a binary
-- chunk. On a pasteboard this action is interpreted as a 'paste' if
-- there is ongoing mouse-drag action on the parent segment, it will
-- be interpreted as a drop. On a segment that announced bchunkstate
-- it will be interpreted as an open operation.
-- If a *description* is provided, a short (~60ch) text type descriptor
-- of the data can be amended. The convention is to treat this as a
-- simplified MIME- type, with the application/ prefix being the default
-- (and can thus be omitted).
-- @note: The *namespace* is one of the set:
-- SHARED_RESOURCE, APPL_RESOURCE, APPL_TEMP_RESOURCE
-- @note: It is possible that the target can re-open the descriptor
-- in a write- mode depending on operating system and creative use
-- of primitives.
-- @group: targetcontrol
-- @cfunction: targetrestore
