# 2026-04-03T02:19:44.780292
import vitis

client = vitis.create_client()
client.set_workspace(path="Vitis")

platform = client.create_platform_component(name = "platform_LED",hw_design = "$COMPONENT_LOCATION/../../SoC/project_1_LED/design_1_LED_wrapper.xsa",os = "standalone",cpu = "microblaze_riscv_0",domain_name = "standalone_microblaze_riscv_0")

comp = client.create_app_component(name="LED",platform = "$COMPONENT_LOCATION/../platform_LED/export/platform_LED/platform_LED.xpfm",domain = "standalone_microblaze_riscv_0",template = "hello_world")

platform = client.get_component(name="platform_LED")
status = platform.build()

status = platform.build()

status = platform.build()

comp = client.get_component(name="LED")
comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

vitis.dispose()

