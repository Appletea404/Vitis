# 2026-04-07T11:02:53.002451
import vitis

client = vitis.create_client()
client.set_workspace(path="Vitis")

platform = client.get_component(name="platform_FND_BUTTON")
status = platform.build()

comp = client.get_component(name="FND_BUTTON")
comp.build()

status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../SoC/project_1_FND/design_1_FND_wrapper.xsa")

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

platform = client.get_component(name="platform_FND")
status = platform.build()

comp = client.get_component(name="hello_world_FND")
comp.build()

status = platform.build()

comp.build()

vitis.dispose()

