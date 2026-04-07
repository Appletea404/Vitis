# 2026-04-07T14:19:45.015155
import vitis

client = vitis.create_client()
client.set_workspace(path="Vitis")

platform = client.get_component(name="platform_FND_BUTTON")
status = platform.build()

comp = client.get_component(name="FND_BUTTON")
comp.build()

status = platform.build()

comp.build()

