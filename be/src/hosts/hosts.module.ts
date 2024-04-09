import { Module, OnModuleInit } from '@nestjs/common';
import { HostsService } from './hosts.service';
import { HttpModule } from '@nestjs/axios';
import { ConfigModule } from '@nestjs/config';
import { ScheduleModule } from '@nestjs/schedule';
import { HostsController } from './hosts.controller';

@Module({
  imports: [HttpModule, ConfigModule, ScheduleModule],
  providers: [HostsService],
  exports: [HostsService],
  controllers: [HostsController],
})
export class HostsModule implements OnModuleInit {
  constructor(private hostsService: HostsService) {}

  onModuleInit() {
    this.hostsService.getAllHosts();
    this.hostsService.getActiveUserSessions();
    this.hostsService.filterHostsToActive();
  }
}
