# 2026-04-07T10:24:11.035930
import vitis

client = vitis.create_client()
client.set_workspace(path="Vitis")

platform = client.get_component(name="platform_FND")
status = platform.build()

comp = client.get_component(name="hello_world_FND")
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

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

platform = client.create_platform_component(name = "platform_FND_BUTTON",hw_design = "$COMPONENT_LOCATION/../../SoC/project_1_FND/design_1_FND_wrapper.xsa",os = "standalone",cpu = "microblaze_riscv_0",domain_name = "standalone_microblaze_riscv_0")

comp = client.create_app_component(name="FND_BUTTON",platform = "$COMPONENT_LOCATION/../platform_FND_BUTTON/export/platform_FND_BUTTON/platform_FND_BUTTON.xpfm",domain = "standalone_microblaze_riscv_0",template = "hello_world")

platform = client.get_component(name="platform_FND_BUTTON")
status = platform.build()

comp = client.get_component(name="FND_BUTTON")
comp.build()

platform = client.get_component(name="platform_BUTTON")
status = platform.build()

comp = client.get_component(name="BUTTON")
comp.build()

status = platform.build()

comp.build()

platform = client.get_component(name="platform_FND_BUTTON")
status = platform.build()

comp = client.get_component(name="FND_BUTTON")
comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

vitis.dispose()

