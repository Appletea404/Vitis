# 2026-04-08T10:28:58.030034
import vitis

client = vitis.create_client()
client.set_workspace(path="Vitis")

platform = client.get_component(name="platform_TIMER")
status = platform.build()

comp = client.get_component(name="hello_world_TIMER")
comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

