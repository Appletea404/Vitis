# 2026-04-03T10:59:14.075571
import vitis

client = vitis.create_client()
client.set_workspace(path="Vitis")

platform = client.get_component(name="platform_LED")
status = platform.build()

comp = client.get_component(name="LED")
comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

platform = client.create_platform_component(name = "platform_BUTTON",hw_design = "$COMPONENT_LOCATION/../../SoC/project_1_BUTTON/design_1_BUTTON_wrapper.xsa",os = "standalone",cpu = "microblaze_riscv_0",domain_name = "standalone_microblaze_riscv_0")

platform = client.get_component(name="platform_BUTTON")
status = platform.build()

comp = client.create_app_component(name="BUTTON",platform = "$COMPONENT_LOCATION/../platform_BUTTON/export/platform_BUTTON/platform_BUTTON.xpfm",domain = "standalone_microblaze_riscv_0",template = "hello_world")

status = platform.build()

comp = client.get_component(name="BUTTON")
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

vitis.dispose()

