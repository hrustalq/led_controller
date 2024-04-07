import { Module } from '@nestjs/common';
import { AppController } from './app.controller';
import { AppService } from './app.service';
import { ControllerGateway } from './controller.gateway';

@Module({
  imports: [],
  controllers: [AppController],
  providers: [AppService, ControllerGateway],
})
export class AppModule {}
