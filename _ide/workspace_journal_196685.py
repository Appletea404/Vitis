# 2026-04-08T11:22:39.640481
import vitis

client = vitis.create_client()
client.set_workspace(path="Vitis")

comp = client.create_app_component(name="hello_world_MILLIS",platform = "$COMPONENT_LOCATION/../platform_TIMER/export/platform_TIMER/platform_TIMER.xpfm",domain = "standalone_microblaze_riscv_0",template = "hello_world")

platform = client.get_component(name="platform_TIMER")
status = platform.build()

comp = client.get_component(name="hello_world_MILLIS")
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

status = platform.build()

comp.build()

platform = client.create_platform_component(name = "platform_PWM",hw_design = "$COMPONENT_LOCATION/../../SoC/project_1_PWM/design_1_PWM_wrapper.xsa",os = "standalone",cpu = "microblaze_riscv_0",domain_name = "standalone_microblaze_riscv_0")

platform = client.get_component(name="platform_PWM")
status = platform.build()

comp = client.create_app_component(name="hello_world_PWM",platform = "$COMPONENT_LOCATION/../platform_PWM/export/platform_PWM/platform_PWM.xpfm",domain = "standalone_microblaze_riscv_0",template = "hello_world")

status = platform.build()

status = platform.build()

status = platform.build()

status = platform.build()

comp = client.get_component(name="hello_world_PWM")
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

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../SoC/project_1_PWM/design_1_PWM_wrapper.xsa")

status = platform.build()

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

vitis.dispose()

